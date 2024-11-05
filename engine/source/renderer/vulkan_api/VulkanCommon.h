#ifndef VULKAN_COMMON_H
#define VULKAN_COMMON_H

#include <vulkan/vulkan.h>
#include "core/logger/Log.h"

namespace Utils {
	inline void VkCheck(const VkResult result, const std::string&& sMessage) {
		if (result != VK_SUCCESS) {
			BROTHER_CORE_ERROR(sMessage);
			throw std::exception(sMessage.c_str());
		}
	}
}

#endif // !VULKAN_COMMON_H
