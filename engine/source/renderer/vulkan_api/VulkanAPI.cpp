#include "VulkanAPI.h"

#include <glfw/glfw3.h>
#include <algorithm>
#include <ranges>
#include <unordered_set>
#include <vector>
#include <array>

#include "core/logger/Log.h"

#include <platform/Window.h>

namespace Brotherhood {
	VulkanAPI::VulkanAPI() {
		CreateInstance();
#ifdef DEBUG
		CreateValidationLayer();
#endif // DEBUG
		PickPhysicalDevice();
		CreateDevice();
		CreateVmaAllocator();
		BROTHER_CORE_TRACE("VulkanAPI created")
	}

	VulkanAPI::~VulkanAPI() {
		vkDestroySwapchainKHR(m_pDevice, m_Swapchain, nullptr);
		vmaDestroyAllocator(m_pAllocator);
		vkDestroyDevice(m_pDevice, nullptr);
		vkDestroySurfaceKHR(m_pInstance, m_Swapchain.Surface, nullptr);
#ifdef DEBUG
		DestroyValidationLayer();
#endif // DEBUG
		vkDestroyInstance(m_pInstance, nullptr);
		BROTHER_CORE_TRACE("VulkanAPI deleted")
	}

#ifdef DEBUG
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
#endif // DEBUG

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
				m_Swapchain.Surface,
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

#ifdef DEBUG
		if (CheckValidationLayerSupport(layerNames)) {
			extensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
#endif // DEBUG

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
				&m_Swapchain.Surface),
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

		for (const auto& extension : availableExtensions) {
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

	void VulkanAPI::ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& cap) {
		uint32_t width { std::clamp(
			Renderer::s_pWindow->GetWidth(),
			cap.minImageExtent.width,
			cap.maxImageExtent.width) };
		uint32_t height { std::clamp(
			Renderer::s_pWindow->GetHeight(),
			cap.minImageExtent.height,
			cap.maxImageExtent.height) };

		m_Swapchain.Extent = { width, height };
	}

	void VulkanAPI::ChooseSwapchainSurfaceFormat() {
		uint32_t surfaceFormatsCount { 0 };
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			m_pPhysicalDevice,
			m_Swapchain.Surface,
			&surfaceFormatsCount,
			nullptr);

		Utils::Check(surfaceFormatsCount == 0, "Vulkan: Failed to get surface formats");

		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			m_pPhysicalDevice,
			m_Swapchain.Surface,
			&surfaceFormatsCount,
			surfaceFormats.data());

		Utils::Check(surfaceFormats.empty(), "Vulkan: Failed to get surface formats");

		for (const auto& format : surfaceFormats) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM
				&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_Swapchain.SurfaceFormat = format;
				return;
			}
		}

		Utils::Check(true, "Vulkan: Failed to get requied surface format");
	}

	void VulkanAPI::ChooseSwapchainPresentationModeFormat() {
		uint32_t presentModesCount { 0 };
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			m_pPhysicalDevice,
			m_Swapchain.Surface,
			&presentModesCount,
			nullptr);

		Utils::Check(presentModesCount == 0, "Vulkan: Failed to get present modes");

		std::vector<VkPresentModeKHR> presentModes(presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			m_pPhysicalDevice,
			m_Swapchain.Surface,
			&presentModesCount,
			presentModes.data());

		Utils::Check(presentModes.empty(), "Vulkan: Failed to get present modes");

		for (const auto& mode : presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				m_Swapchain.PresentMode = mode;
				return;
			}
		}

		Utils::Check(true, "Vulkan: Failed to get requied present modes");
	}

	void VulkanAPI::ChooseSwapchainImageCount(const VkSurfaceCapabilitiesKHR& cap) {
		m_Swapchain.ImageCount = cap.minImageCount + 1;
		if (cap.maxImageCount > 0 && m_Swapchain.ImageCount > cap.maxImageCount) {
			m_Swapchain.ImageCount = cap.maxImageCount;
		}
	}

	void VulkanAPI::CreateSwapchain() {
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			m_pPhysicalDevice,
			m_Swapchain.Surface,
			&capabilities);

		ChooseSwapchainExtent(capabilities);
		ChooseSwapchainSurfaceFormat();
		ChooseSwapchainPresentationModeFormat();
		ChooseSwapchainImageCount(capabilities);

		const auto& swapchain = m_Swapchain;
		VkSwapchainCreateInfoKHR swapchainCreateInfo {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.flags = {};
		swapchainCreateInfo.surface = m_Swapchain.Surface;
		swapchainCreateInfo.minImageCount = m_Swapchain.ImageCount;
		swapchainCreateInfo.imageFormat = m_Swapchain.SurfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = m_Swapchain.SurfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = m_Swapchain.Extent;
		swapchainCreateInfo.imageArrayLayers = 1; // CHECK
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // CHECK

		const std::unordered_set<uint32_t> queueIndicesSet { m_Indices.Compute.value(),
															 m_Indices.Graphics.value(),
															 m_Indices.Present.value(),
															 m_Indices.Transfer.value() };

		const std::vector<uint32_t> queueIndicesVec(
			queueIndicesSet.begin(),
			queueIndicesSet.end());

		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount =
			static_cast<uint32_t>(queueIndicesVec.size());
		swapchainCreateInfo.pQueueFamilyIndices = queueIndicesVec.data();
		swapchainCreateInfo.preTransform = capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = m_Swapchain.PresentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		Utils::VkCheck(
			vkCreateSwapchainKHR(m_pDevice, &swapchainCreateInfo, nullptr, m_Swapchain),
			"Vulkan: Failed to create swapchain");
		BROTHER_CORE_TRACE("Vulkan swapchain created")

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(m_pDevice, m_Swapchain, &imageCount, nullptr);
		m_Swapchain.Images.resize(imageCount);
		vkGetSwapchainImagesKHR(
			m_pDevice,
			m_Swapchain,
			&imageCount,
			m_Swapchain.Images.data());
	}

	VkFormat VulkanAPI::FindSupportedFormat(
		const std::vector<VkFormat>& formats,
		const VkImageTiling tiling,
		const VkFormatFeatureFlags features) {
		for (auto&& format : formats) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_pPhysicalDevice, format, &props);

			if (VK_IMAGE_TILING_LINEAR == tiling
				&& (props.linearTilingFeatures & features) == features) {
				return format;
			} else if (
				VK_IMAGE_TILING_OPTIMAL == tiling
				&& (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat VulkanAPI::FindDepthFormat() {
		return FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT,
			  VK_FORMAT_D32_SFLOAT_S8_UINT,
			  VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	void VulkanAPI::CreateFrameBuffers() {
		{
			VkImageCreateInfo colorImageCreateInfo {};
			colorImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			colorImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			colorImageCreateInfo.extent.width = m_Swapchain.Extent.width;
			colorImageCreateInfo.extent.height = m_Swapchain.Extent.height;
			colorImageCreateInfo.extent.depth = 1;
			colorImageCreateInfo.mipLevels = 1;
			colorImageCreateInfo.arrayLayers = 1;
			colorImageCreateInfo.format = m_Swapchain.SurfaceFormat.format;
			colorImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			colorImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			colorImageCreateInfo.samples = m_MaxSampleCount;
			colorImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo colorAllocCreateInfo {};
			colorAllocCreateInfo.flags = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // CHECK
			colorAllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			VkResult result = vmaCreateImage(
				m_pAllocator,
				&colorImageCreateInfo,
				&colorAllocCreateInfo,
				&m_FrameBuffers.Color.Image,
				&m_FrameBuffers.Color.Allocation,
				nullptr);
			Utils::VkCheck(
				result,
				std::format("Message: {}", "Vulkan: Failed to create color image"));

			VkImageViewCreateInfo ColorViewInfo {};
			ColorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ColorViewInfo.image = m_FrameBuffers.Color.Image;
			ColorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ColorViewInfo.format = m_Swapchain.SurfaceFormat.format;
			ColorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ColorViewInfo.subresourceRange.baseMipLevel = 0;
			ColorViewInfo.subresourceRange.levelCount = 1;
			ColorViewInfo.subresourceRange.baseArrayLayer = 0;
			ColorViewInfo.subresourceRange.layerCount = 1;

			result = vkCreateImageView(
				m_pDevice,
				&ColorViewInfo,
				nullptr,
				&m_FrameBuffers.Color.ImageView);
			Utils::VkCheck(
				result,
				std::format("Message: {}", "Vulkan: Failed to create image view"));
		}

		{
			VkImageCreateInfo depthImageCreateInfo {};
			depthImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			depthImageCreateInfo.extent.width = m_Swapchain.Extent.width;
			depthImageCreateInfo.extent.height = m_Swapchain.Extent.height;
			depthImageCreateInfo.extent.depth = 1;
			depthImageCreateInfo.mipLevels = 1;
			depthImageCreateInfo.arrayLayers = 1;
			depthImageCreateInfo.format = FindDepthFormat();
			depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			depthImageCreateInfo.samples = m_MaxSampleCount;
			depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo depthAllocCreateInfo {};
			depthAllocCreateInfo.flags = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // CHECK
			depthAllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			VkResult result = vmaCreateImage(
				m_pAllocator,
				&depthImageCreateInfo,
				&depthAllocCreateInfo,
				&m_FrameBuffers.Depth.Image,
				&m_FrameBuffers.Depth.Allocation,
				nullptr);
			Utils::VkCheck(
				result,
				std::format("Message: {}", "Vulkan: Failed to create color image"));

			VkImageViewCreateInfo DepthViewInfo {};
			DepthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			DepthViewInfo.image = m_FrameBuffers.Color.Image;
			DepthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			DepthViewInfo.format = FindDepthFormat();
			DepthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			DepthViewInfo.subresourceRange.baseMipLevel = 0;
			DepthViewInfo.subresourceRange.levelCount = 1;
			DepthViewInfo.subresourceRange.baseArrayLayer = 0;
			DepthViewInfo.subresourceRange.layerCount = 1;

			result = vkCreateImageView(
				m_pDevice,
				&DepthViewInfo,
				nullptr,
				&m_FrameBuffers.Color.ImageView);
			Utils::VkCheck(
				result,
				std::format("Message: {}", "Vulkan: Failed to create depth view"));
		}

		{
			m_FrameBuffers.Resolve.reserve(m_Swapchain.ImageCount);
			for (size_t nIndex = 0; nIndex < m_Swapchain.ImageCount; nIndex++) {
				std::array<VkImageView, 3> attachments = {
					m_FrameBuffers.Color.ImageView,
					m_FrameBuffers.Depth.ImageView,
					m_FrameBuffers.Resolve[nIndex].ImageView
				};

				VkFramebufferCreateInfo framebufferInfo {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = PIPELINE.renderPass;
				framebufferInfo.attachmentCount =
					static_cast<uint32_t>(attachments.size());
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = DEVICE.SWAPCHAIN.extent.width;
				framebufferInfo.height = DEVICE.SWAPCHAIN.extent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(
						DEVICE,
						&framebufferInfo,
						nullptr,
						&DEVICE.SWAPCHAIN.swapchainFrameBuffer[nIndex])
					!= VK_SUCCESS) {
					throw std::runtime_error("failed to create framebuffer!");
				}
			}
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

#ifdef DEBUG
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
#endif // DEBUG
} // Brotherhood
