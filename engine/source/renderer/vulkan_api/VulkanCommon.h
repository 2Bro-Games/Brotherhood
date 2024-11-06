#ifndef VULKAN_COMMON_H
#define VULKAN_COMMON_H

#include <vulkan/vulkan.h>
#include <iostream>
#include "core/logger/Log.h"

namespace Utils {
	inline void VkCheck(const VkResult result, const std::string&& sMessage) {
		if (result != VK_SUCCESS) {
			BROTHER_CORE_ERROR(sMessage);
			throw std::exception(sMessage.c_str());
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
}

#endif // !VULKAN_COMMON_H
