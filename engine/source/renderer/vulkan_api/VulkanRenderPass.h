#ifndef VULKANRENDERPASS_H
#define VULKANRENDERPASS_H

#include "VulkanCommon.h"

namespace Brotherhood {
	using VulkanAttachmentIndex = uint32_t;
	using VulkanSubpassDescriptionIndex = uint32_t;
	using VulkanSubpassDependencyIndex = uint32_t;

	using VulkanAttachmentIndices = std::vector<VulkanAttachmentIndex>;

	class VulkanRenderPass {
	public:
		VulkanRenderPass(VkDevice device) : m_Device(device) {
		}

		~VulkanRenderPass();

		VulkanAttachmentIndex AddAttachment(
			const VkAttachmentDescription& desc,
			VkImageLayout layout);

		VulkanSubpassDescriptionIndex AddDescription(
			const VkSubpassDescription& createInfo);

		VulkanSubpassDependencyIndex AddDependency(const VkSubpassDependency& depend);


	private:
		void CreateRenderPass();

		std::vector<VkAttachmentReference> CreateAttachmentReference(
			const VulkanAttachmentIndices& indices);

		VkDevice m_Device { nullptr };

		std::vector<VkAttachmentDescription> m_Attachments;
		std::unordered_map<VulkanAttachmentIndex, VkAttachmentReference> m_RefMap;
		std::vector<VkSubpassDescription> m_SubpassDescriptions;
		std::vector<VkSubpassDependency> m_SubpassDependency;

		VkRenderPass m_RenderPass { nullptr };
	};
} // Brotherhood
#endif // VULKANRENDERPASS_H