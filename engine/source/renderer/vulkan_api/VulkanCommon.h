#ifndef VULKAN_COMMON_H
#define VULKAN_COMMON_H

#include <vulkan/vulkan.h>

#include "core/Common.h"
#include "vk_mem_alloc.h"

constexpr uint32_t g_BuffersCount = 2;

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
		BROTHER_ERROR(std::format("Validation layer: %s", pCallbackData->pMessage));
		return VK_FALSE;
	}
#endif // DEBUG
}

#endif // !VULKAN_COMMON_H
