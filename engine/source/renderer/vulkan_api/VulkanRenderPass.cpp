#include "VulkanRenderPass.h"

namespace Brotherhood {
	VulkanRenderPass::~VulkanRenderPass() {
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	}

	VulkanAttachmentIndex VulkanRenderPass::AddAttachment(
		const VkAttachmentDescription& desc,
		VkImageLayout layout) {
		const auto index = static_cast<VulkanAttachmentIndex>(m_Attachments.size());
		m_Attachments.emplace_back(desc);

		VkAttachmentReference attachmentRef {};
		attachmentRef.attachment = index;
		attachmentRef.layout = layout;
		m_RefMap[index] = { std::move(attachmentRef) };
		return index;
	}

	VulkanSubpassDescriptionIndex VulkanRenderPass::AddDescription(
		const VkSubpassDescription& createInfo) {
		const auto index =
			static_cast<VulkanSubpassDescriptionIndex>(m_SubpassDescriptions.size());
		m_SubpassDescriptions.emplace_back(createInfo);
		return index;
	}

	VulkanSubpassDependencyIndex VulkanRenderPass::AddDependency(
		const VkSubpassDependency& depend) {
		const auto index =
			static_cast<VulkanSubpassDependencyIndex>(m_SubpassDependency.size());
		m_SubpassDependency.emplace_back(depend);
		return index;
	}

	void VulkanRenderPass::CreateRenderPass() {
		VkRenderPassCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.attachmentCount = static_cast<uint32_t>(m_Attachments.size());
		createInfo.pAttachments = m_Attachments.data();
		createInfo.subpassCount = static_cast<uint32_t>(m_SubpassDescriptions.size());
		createInfo.pSubpasses = m_SubpassDescriptions.data();
		createInfo.dependencyCount = static_cast<uint32_t>(m_SubpassDependency.size());
		createInfo.pDependencies = m_SubpassDependency.data();

		const VkResult result =
			vkCreateRenderPass(m_Device, &createInfo, nullptr, &m_RenderPass);

		Utils::VkCheck(
			result,
			std::format("Message: {}", "Vulkan: Failed to create render pass"));
	}

	std::vector<VkAttachmentReference> VulkanRenderPass::CreateAttachmentReference(
		const VulkanAttachmentIndices& indices) {
		std::vector<VkAttachmentReference> refs;
		for (auto index : indices) {
			refs.emplace_back(m_RefMap[index]);
		}
		return refs;
	}
} // Brotherhood