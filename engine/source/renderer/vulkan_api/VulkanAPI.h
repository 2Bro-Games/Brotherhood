#ifndef VULKAN_API_H
#define VULKAN_API_H

#include <optional>

#include "renderer/RendererAPI.h"

#include "VulkanCommon.h"
#include "vk_mem_alloc.h"

#include "glfw/glfw3.h"

class Renderer;

namespace Brotherhood {
	class VulkanAPI final : public Renderer {
	public:
		VulkanAPI();
		virtual ~VulkanAPI() override;

	private:
		void CreateInstance();

		void PickPhysicalDevice();
		void PickQueueIndices();
		static uint32_t RateDeviceSuitability(VkPhysicalDevice pPhDevice_);
		void CreateDevice();

		void DefineMaxSampleCount();

		void CreateVmaAllocator();

		void CreateValidationLayer();
		void DestroyValidationLayer();

	private:
		VkInstance m_pInstance { nullptr };
		VkPhysicalDevice m_pPhysicalDevice { nullptr };
		VkDevice m_pDevice { nullptr };

		VkSurfaceKHR m_pSurface { nullptr };

		VmaAllocator m_pAllocator { nullptr };

		struct {
			std::optional<uint32_t> Compute;
			std::optional<uint32_t> Graphics;
			std::optional<uint32_t> Present;
			std::optional<uint32_t> Transfer;
		} m_Indices;

		VkSampleCountFlagBits m_MaxSampleCount { VK_SAMPLE_COUNT_1_BIT };

		VkDebugUtilsMessengerEXT m_pDebugMessenger { nullptr };
	};
}
#endif // !VULKAN_API_H
