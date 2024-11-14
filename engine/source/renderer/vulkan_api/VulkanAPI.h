#ifndef VULKAN_API_H
#define VULKAN_API_H

#include <optional>
#include <vector>

#include "renderer/RendererAPI.h"

#include "VulkanCommon.h"

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
		void CreateDevice();
		void DefineMaxSampleCount();

		void CreateVmaAllocator();

		void ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& cap);
		void ChooseSwapchainSurfaceFormat();
		void ChooseSwapchainPresentationModeFormat();
		void ChooseSwapchainImageCount(const VkSurfaceCapabilitiesKHR& cap);
		void CreateSwapchain();

		VkFormat FindSupportedFormat(
			const std::vector<VkFormat>& formats,
			const VkImageTiling tiling,
			const VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		void CreateFrameBuffers();

		static uint32_t RateDeviceSuitability(VkPhysicalDevice pPhDevice_);

#ifdef DEBUG
		void CreateValidationLayer();
		void DestroyValidationLayer();
#endif // DEBUG

	private:
		VkInstance m_pInstance { nullptr };
		VkPhysicalDevice m_pPhysicalDevice { nullptr };
		VkDevice m_pDevice { nullptr };

		struct {
			VkSurfaceKHR Surface { nullptr };
			VkSwapchainKHR Swapchain { nullptr };

			uint32_t ImageCount;
			uint32_t CurrentImage { 0 };

			VkPresentModeKHR PresentMode;
			VkSurfaceFormatKHR SurfaceFormat;
			VkExtent2D Extent;

			std::vector<VkImage> Images;
			std::vector<VkImageView> ImageViews;
			std::vector<VkFramebuffer> FrameBuffer;

			operator VkSwapchainKHR() const {
				return Swapchain;
			}

			operator VkSwapchainKHR*() {
				return &Swapchain;
			}
		} m_Swapchain;

		struct VkImageContainer {
			VkImage Image;
			VkImageView ImageView;
			VmaAllocation Allocation;
		};

		struct {
			uint32_t CurrentBuffer;

			VkImageContainer Color;
			VkImageContainer Depth;
			std::vector<VkImageContainer> Resolve;

		} m_FrameBuffers;

		VmaAllocator m_pAllocator { nullptr };

		struct {
			std::optional<uint32_t> Compute;
			std::optional<uint32_t> Graphics;
			std::optional<uint32_t> Present;
			std::optional<uint32_t> Transfer;
		} m_Indices;

		VkSampleCountFlagBits m_MaxSampleCount { VK_SAMPLE_COUNT_1_BIT };

#ifdef DEBUG
		VkDebugUtilsMessengerEXT m_pDebugMessenger { nullptr };
#endif // DEBUG
	};
}
#endif // !VULKAN_API_H
