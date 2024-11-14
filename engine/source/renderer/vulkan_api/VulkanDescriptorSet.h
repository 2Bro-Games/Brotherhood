#ifndef VULKANDESCRIPTORSET_H
#define VULKANDESCRIPTORSET_H

#include "VulkanCommon.h"

#include <vector>
#include <unordered_map>

namespace Brotherhood {
	class VulkanDescriptorSet {
	public:
		VulkanDescriptorSet(VkDevice device) : m_Device(device) {
		}

		~VulkanDescriptorSet();

		void AddDescriptorSetLayout(
			const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const {
			return m_SetLayouts;
		}

		void UpdateDescriptorSets(std::vector<VkWriteDescriptorSet>& descriptorWrites);

	private:
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		VkDevice m_Device { nullptr };

		std::unordered_map<VkDescriptorType, uint32_t> m_TypesCount;

		std::vector<VkDescriptorSetLayout> m_SetLayouts;
		VkDescriptorPool m_Pool { nullptr };
		std::vector<VkDescriptorSet> m_Sets;
	};
} // Brotherhood

#endif // VULKANDESCRIPTORSET_H
