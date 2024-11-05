#ifndef VULKAN_API_H
#define VULKAN_API_H

#include <vector>
#include <unordered_set>

#include "Render/RendererAPI.h"
#include "VulkanCommon.h"
#include "vk_mem_alloc.h"

class Renderer;

namespace Brotherhood {
	enum class QueueIndex {
		eGraphics,
		ePresent,
		eCompute,
		eTransfer
	};

	struct VulkanQueueIndices {
	public:
		uint32_t graphicsQueueIndex{ UINT32_MAX };
		uint32_t presentQueueIndex{ UINT32_MAX };
		uint32_t computeQueueIndex{ UINT32_MAX };
		uint32_t transferQueueIndex{ UINT32_MAX };

		const uint32_t& operator[](QueueIndex queueIndex_) const {
			switch (queueIndex_) {
			case QueueIndex::eGraphics:
				return graphicsQueueIndex;
			case QueueIndex::ePresent:
				return presentQueueIndex;
			case QueueIndex::eCompute:
				return computeQueueIndex;
			case QueueIndex::eTransfer:
				return transferQueueIndex;
			default:
				return dummy;
			}
		}

	private:
		uint32_t dummy{ UINT16_MAX };
	};

	class VulkanAPI final : public Renderer {
	public:
		VulkanAPI();
		virtual ~VulkanAPI();

	private:
		void CreateInstance() {
			VkApplicationInfo applicationInfo{};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pEngineName = "Brotherhood";
			applicationInfo.pApplicationName = "Brotherhood Application";
			applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
			applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
			applicationInfo.apiVersion = VK_API_VERSION_1_3;

			VkInstanceCreateInfo instanceCreateInfo{};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pApplicationInfo = &applicationInfo;

			Utils::VkCheck(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance),
				"Vulkan instance not created");
			BROTHER_CORE_TRACE("Vulkan instance created")
		};

		void PickPhysicalDevice() {
			uint32_t physicalDeviceCount{ 0 };
			vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);

			std::vector<VkPhysicalDevice> physicalDevices{ physicalDeviceCount };
			vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());

			uint32_t rateScore{ 0 };
			for (const auto& phDevice : physicalDevices) {
				const uint32_t score = RateDeviceSuitability(phDevice);
				if (rateScore < score) {
					rateScore = score;
					m_PhysicalDevice = phDevice;
				}
			}

			if (!m_PhysicalDevice) {
				throw std::exception("Physical Device was not picked");
			}

			//if (!PickQueueIndecies(m_PhysicalDevice, surface_)) {
			//	throw std::exception("Graphics Family Queue was not found");
			//}

			DefineMaxSampleCount();
		};

		bool PickQueueIndecies(VkPhysicalDevice pDevice, VkSurfaceKHR pSurface)
		{

		}

		uint32_t RateDeviceSuitability(VkPhysicalDevice phDevice_) {
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(phDevice_, &features);

			if (features.geometryShader != VK_TRUE ||
				features.samplerAnisotropy != VK_TRUE) {
				return 0;
			}

			// Check other features ...

			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(phDevice_, &properties);

			uint32_t score{ 0 };
			bool suitable = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
			score += 100 * suitable;

#ifndef NDEBUG
			suitable = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
			score += 10 * suitable;
#endif // NDEBUG

			// Check other properties ...

			uint32_t availableExtensionsCount{};
			vkEnumerateDeviceExtensionProperties(phDevice_,
				nullptr, &availableExtensionsCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
			vkEnumerateDeviceExtensionProperties(phDevice_, nullptr,
				&availableExtensionsCount, availableExtensions.data());

			const std::vector<const char*> deviceExtensions{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			std::unordered_set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

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

		void CreateDevice() {
			// TODO: Support transfer and compute
			std::unordered_set<uint32_t> uniqueIndices{
				m_QueueIndices.graphicsQueueIndex, m_QueueIndices.presentQueueIndex };

			std::vector<float> queuePriorities{ 1.f };
			std::vector<VkDeviceQueueCreateInfo> queueCIs;
			queueCIs.reserve(uniqueIndices.size());
			VkDeviceQueueCreateInfo queueCI{};
			queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCI.queueCount = 1;
			queueCI.pQueuePriorities = queuePriorities.data();
			for (auto&& index : uniqueIndices) {
				queueCI.queueFamilyIndex = index;
				queueCIs.emplace_back(queueCI);
			}

			std::vector<const char*> layers{};
			std::vector<const char*> extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			VkPhysicalDeviceFeatures features{};
			features.samplerAnisotropy = VK_TRUE;
			features.sampleRateShading = VK_TRUE;

			VkDeviceCreateInfo deviceCI{};
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
				vkCreateDevice(m_PhysicalDevice, &deviceCI, nullptr, &m_Device),
				"Device was not created");
			BROTHER_CORE_TRACE("Vulkan device created")
		};

		void DefineMaxSampleCount() {
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

			VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
			if (counts & VK_SAMPLE_COUNT_64_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_64_BIT; return; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_32_BIT; return; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_16_BIT; return; }
			if (counts & VK_SAMPLE_COUNT_8_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_8_BIT; return; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_4_BIT; return; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { m_MaxSampleCount = VK_SAMPLE_COUNT_2_BIT; return; }
		}

		void CreateVmaAllocator()
		{
			VmaVulkanFunctions vulkanFunctions = {};
			vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
			vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

			VmaAllocatorCreateInfo allocatorCreateInfo = {};
			//allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
			allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			allocatorCreateInfo.physicalDevice = m_PhysicalDevice;
			allocatorCreateInfo.device = m_Device;
			allocatorCreateInfo.instance = m_Instance;
			allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

			Utils::VkCheck(
				vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator),
				"Vma allocator was not created");
			BROTHER_CORE_TRACE("Vma allocator created")
		}

	private:
		VkInstance m_Instance;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VmaAllocator m_Allocator;

		VulkanQueueIndices m_QueueIndices;
		VkSampleCountFlagBits m_MaxSampleCount{ VK_SAMPLE_COUNT_1_BIT };
	};
}

#endif // !VULKAN_API_H
