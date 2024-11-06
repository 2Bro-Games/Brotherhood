#include "VulkanAPI.h"

#include <glfw/glfw3.h>
#include <algorithm>
#include <ranges>
#include <unordered_set>
#include <vector>

#include "core/logger/Log.h"

#include <platform/Window.h>

namespace Brotherhood {
	VulkanAPI::VulkanAPI() {
		CreateInstance();
		CreateValidationLayer();
		PickPhysicalDevice();
		CreateDevice();
		CreateVmaAllocator();
		BROTHER_CORE_TRACE("VulkanAPI created")
	}

	VulkanAPI::~VulkanAPI() {
		vmaDestroyAllocator(m_pAllocator);
		vkDestroyDevice(m_pDevice, nullptr);
		vkDestroySurfaceKHR(m_pInstance, m_pSurface, nullptr);
		DestroyValidationLayer();
		vkDestroyInstance(m_pInstance, nullptr);
		BROTHER_CORE_TRACE("VulkanAPI deleted")
	}

	bool CheckValidationLayerSupport(const std::vector<const char*>& layers) {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : layers) {
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}
			if (!layerFound) {
				return false;
			}
		}
		return true;
	}

	void VulkanAPI::PickQueueIndices() {
		uint32_t queueFamilyPropertiesCount { 0 };
		vkGetPhysicalDeviceQueueFamilyProperties(
			m_pPhysicalDevice,
			&queueFamilyPropertiesCount,
			nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(
			queueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			m_pPhysicalDevice,
			&queueFamilyPropertiesCount,
			queueFamilyProperties.data());

		uint8_t min_score { UINT8_MAX };
		const uint32_t nCount { static_cast<uint32_t>(queueFamilyProperties.size()) };
		for (uint32_t nIndex { 0 }; nIndex < nCount; ++nIndex) {
			uint8_t score { 0 };

			// Graphics queue
			if (!m_Indices.Graphics.has_value()
				&& queueFamilyProperties[nIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				m_Indices.Graphics = nIndex;
				++score;
			}

			// Present queue
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(
				m_pPhysicalDevice,
				nIndex,
				m_pSurface,
				&presentSupport);
			if (!m_Indices.Present.has_value() && presentSupport == VK_TRUE) {
				m_Indices.Present = nIndex;
				++score;
			}

			// Compute queue
			if (queueFamilyProperties[nIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				m_Indices.Compute = nIndex;
				++score;
			}

			// Transfer queue
			if (min_score > score
				&& queueFamilyProperties[nIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				min_score = score;
				m_Indices.Transfer = nIndex;
			}
		}

		if (!m_Indices.Graphics.has_value()) {
			throw std::exception("Vulkan failed to pick graphics index");
		}
	}
	void VulkanAPI::CreateInstance() {
		VkApplicationInfo applicationInfo {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pEngineName = "Brotherhood";
		applicationInfo.pApplicationName = "Brotherhood Application";
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_3;

		std::vector<const char*> extensionNames;
		std::vector<const char*> layerNames;

		uint32_t nCount;
		const char** sNames { glfwGetRequiredInstanceExtensions(&nCount) };
		std::ranges::copy_n(sNames, nCount, std::back_inserter(extensionNames));

		if (CheckValidationLayerSupport(layerNames)) {
			extensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		VkInstanceCreateInfo instanceCreateInfo {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount =
			static_cast<uint32_t>(extensionNames.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;

		Utils::VkCheck(
			vkCreateInstance(&instanceCreateInfo, nullptr, &m_pInstance),
			"Vulkan instance not created");
		BROTHER_CORE_TRACE("Vulkan instance created")

		Utils::VkCheck(
			glfwCreateWindowSurface(
				m_pInstance,
				static_cast<GLFWwindow*>(Renderer::s_pWindow->GetNativeWindow()),
				nullptr,
				&m_pSurface),
			"Vulkan surface not created");
		BROTHER_CORE_TRACE("Vulkan surface created")
	}

	void VulkanAPI::PickPhysicalDevice() {
		uint32_t physicalDeviceCount { 0 };
		vkEnumeratePhysicalDevices(m_pInstance, &physicalDeviceCount, nullptr);

		std::vector<VkPhysicalDevice> physicalDevices { physicalDeviceCount };
		vkEnumeratePhysicalDevices(
			m_pInstance,
			&physicalDeviceCount,
			physicalDevices.data());

		uint32_t rateScore { 0 };
		for (const auto& phDevice : physicalDevices) {
			const uint32_t score = RateDeviceSuitability(phDevice);
			if (rateScore < score) {
				rateScore = score;
				m_pPhysicalDevice = phDevice;
			}
		}

		if (!m_pPhysicalDevice) {
			throw std::exception("Physical Device was not picked");
		}

		PickQueueIndices();
		DefineMaxSampleCount();
	};

	uint32_t VulkanAPI::RateDeviceSuitability(VkPhysicalDevice phDevice_) {
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(phDevice_, &features);

		if (features.geometryShader != VK_TRUE || features.samplerAnisotropy != VK_TRUE) {
			return 0;
		}

		// Check other features ...

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(phDevice_, &properties);

		uint32_t score { 0 };
		bool suitable = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		score += 100 * suitable;

#ifndef NDEBUG
		suitable = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
		score += 10 * suitable;
#endif // NDEBUG

		// Check other properties ...

		uint32_t availableExtensionsCount {};
		vkEnumerateDeviceExtensionProperties(
			phDevice_,
			nullptr,
			&availableExtensionsCount,
			nullptr);

		std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
		vkEnumerateDeviceExtensionProperties(
			phDevice_,
			nullptr,
			&availableExtensionsCount,
			availableExtensions.data());

		const std::vector<const char*> deviceExtensions {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::unordered_set<std::string> requiredExtensions(
			deviceExtensions.begin(),
			deviceExtensions.end());

		for (auto&& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
			if (requiredExtensions.empty()) {
				break;
			}
		}

		if (!requiredExtensions.empty()) {
			return 0;
		}

		return score;
	}

	void VulkanAPI::CreateDevice() {
		// TODO: Support transfer and compute
		std::unordered_set<uint32_t> uniqueIndices { m_Indices.Graphics.value(),
													 m_Indices.Present.value() };

		std::vector<float> queuePriorities { 1.f };
		std::vector<VkDeviceQueueCreateInfo> queueCIs;
		queueCIs.reserve(uniqueIndices.size());
		VkDeviceQueueCreateInfo queueCI {};
		queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCI.queueCount = 1;
		queueCI.pQueuePriorities = queuePriorities.data();
		for (auto&& index : uniqueIndices) {
			queueCI.queueFamilyIndex = index;
			queueCIs.emplace_back(queueCI);
		}

		std::vector<const char*> layers {};
		std::vector<const char*> extensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkPhysicalDeviceFeatures features {};
		features.samplerAnisotropy = VK_TRUE;
		features.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo deviceCI {};
		deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCI.pNext = nullptr;
		deviceCI.flags = 0;
		deviceCI.queueCreateInfoCount = static_cast<uint32_t>(queueCIs.size());
		deviceCI.pQueueCreateInfos = queueCIs.data();
		deviceCI.enabledLayerCount = static_cast<uint32_t>(layers.size());
		deviceCI.ppEnabledLayerNames = layers.data();
		deviceCI.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		deviceCI.ppEnabledExtensionNames = extensions.data();
		deviceCI.pEnabledFeatures = &features;

		Utils::VkCheck(
			vkCreateDevice(m_pPhysicalDevice, &deviceCI, nullptr, &m_pDevice),
			"Device was not created");
		BROTHER_CORE_TRACE("Vulkan device created")
	};

	void VulkanAPI::DefineMaxSampleCount() {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_pPhysicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts =
			physicalDeviceProperties.limits.framebufferColorSampleCounts
			& physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) {
			m_MaxSampleCount = VK_SAMPLE_COUNT_64_BIT;
			return;
		}
		if (counts & VK_SAMPLE_COUNT_32_BIT) {
			m_MaxSampleCount = VK_SAMPLE_COUNT_32_BIT;
			return;
		}
		if (counts & VK_SAMPLE_COUNT_16_BIT) {
			m_MaxSampleCount = VK_SAMPLE_COUNT_16_BIT;
			return;
		}
		if (counts & VK_SAMPLE_COUNT_8_BIT) {
			m_MaxSampleCount = VK_SAMPLE_COUNT_8_BIT;
			return;
		}
		if (counts & VK_SAMPLE_COUNT_4_BIT) {
			m_MaxSampleCount = VK_SAMPLE_COUNT_4_BIT;
			return;
		}
		if (counts & VK_SAMPLE_COUNT_2_BIT) {
			m_MaxSampleCount = VK_SAMPLE_COUNT_2_BIT;
			return;
		}
	}

	void VulkanAPI::CreateVmaAllocator() {
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		//allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorCreateInfo.physicalDevice = m_pPhysicalDevice;
		allocatorCreateInfo.device = m_pDevice;
		allocatorCreateInfo.instance = m_pInstance;
		allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

		Utils::VkCheck(
			vmaCreateAllocator(&allocatorCreateInfo, &m_pAllocator),
			"Vma allocator was not created");
		BROTHER_CORE_TRACE("Vma allocator created")
	}

	void VulkanAPI::CreateValidationLayer() {
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = Utils::DebugCallback;

		auto createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(m_pInstance, "vkCreateDebugUtilsMessengerEXT");
		if (createDebugUtilsMessengerEXT != nullptr) {
			createDebugUtilsMessengerEXT(
				m_pInstance,
				&debugCreateInfo,
				nullptr,
				&m_pDebugMessenger);
		}
	}

	void VulkanAPI::DestroyValidationLayer() {
		auto destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(m_pInstance, "vkDestroyDebugUtilsMessengerEXT");
		if (destroyDebugUtilsMessengerEXT != nullptr) {
			destroyDebugUtilsMessengerEXT(m_pInstance, m_pDebugMessenger, nullptr);
		}
	}
} // Brotherhood
