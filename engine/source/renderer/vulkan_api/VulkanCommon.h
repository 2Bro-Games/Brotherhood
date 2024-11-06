#ifndef VULKAN_COMMON_H
#define VULKAN_COMMON_H

#include "core/Common.h"
#include "core/logger/Log.h"

#include <vulkan/vulkan.h>
#include <iostream>

namespace Utils {
	inline void VkCheck(const VkResult result, std::string&& sMessage) {
		Check(result != VK_SUCCESS, std::move(sMessage));
	}

#ifdef DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
#endif // DEBUG
}

#endif // !VULKAN_COMMON_H
