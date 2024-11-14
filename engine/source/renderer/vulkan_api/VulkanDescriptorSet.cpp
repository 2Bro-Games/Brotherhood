#include "VulkanDescriptorSet.h"

namespace Brotherhood {
	VulkanDescriptorSet::~VulkanDescriptorSet() {
		vkFreeDescriptorSets(m_Device, m_Pool, m_Sets.size(), m_Sets.data());
		vkDestroyDescriptorPool(m_Device, m_Pool, nullptr);
		for (const auto setLayout : m_SetLayouts) {
			vkDestroyDescriptorSetLayout(m_Device, setLayout, nullptr);
		}
	}

	void VulkanDescriptorSet::AddDescriptorSetLayout(
		const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
		for (const auto& binding : bindings) {
			m_TypesCount[binding.descriptorType] += binding.descriptorCount;
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI {};
		descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCI.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptorSetLayoutCI.pBindings = bindings.data();

		m_SetLayouts.emplace_back();
		const VkResult result = vkCreateDescriptorSetLayout(
			m_Device,
			&descriptorSetLayoutCI,
			nullptr,
			&m_SetLayouts[m_SetLayouts.size() - 1]);
		Utils::VkCheck(
			result,
			std::format("Message: {}", "Vulkan: Failed to create descriptor set layout"));
	}

	void VulkanDescriptorSet::UpdateDescriptorSets(
		std::vector<VkWriteDescriptorSet>& descriptorWrites) {
		for (uint32_t nBufferIndex = 0; nBufferIndex < g_BuffersCount; ++nBufferIndex) {
			for (size_t nIndex = 0; nIndex < descriptorWrites.size(); ++nIndex) {
				descriptorWrites[nIndex].dstSet = m_Sets[nIndex];
			}
			vkUpdateDescriptorSets(
				m_Device,
				static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr);
		}
	}

	void VulkanDescriptorSet::CreateDescriptorPool() {
		std::vector<VkDescriptorPoolSize> poolSizes(m_TypesCount.size());
		auto begin = poolSizes.begin();
		for (const auto& descriptor : m_TypesCount) {
			begin->type = descriptor.first;
			begin->descriptorCount = descriptor.second * g_BuffersCount;
			++begin;
		}

		VkDescriptorPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(g_BuffersCount);

		Utils::VkCheck(
			vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_Pool),
			std::format("Message: {}", "Vulkan: Failed to create descriptor pool"));
	}

	void VulkanDescriptorSet::CreateDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts;
		layouts.reserve(m_SetLayouts.size() * 2);
		for (const auto layout : m_SetLayouts) {
			for (int nIndex = 0; nIndex < g_BuffersCount; ++nIndex) {
				layouts.emplace_back(layout);
			}
		}
		VkDescriptorSetAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_Pool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(g_BuffersCount);
		allocInfo.pSetLayouts = layouts.data();

		m_Sets.emplace_back();
		m_Sets.resize(g_BuffersCount);
		Utils::VkCheck(
			vkAllocateDescriptorSets(m_Device, &allocInfo, m_Sets.data()),
			std::format("Message: {}", "Vulkan: Failed to create descriptor sets"));
	}
} // Brotherhood