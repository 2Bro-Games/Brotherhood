#include "VulkanAPI.h"
#include "core/logger/Log.h"

Brotherhood::VulkanAPI::VulkanAPI()
{
	CreateInstance();
	PickPhysicalDevice();
	CreateDevice();
	CreateVmaAllocator();
	BROTHER_CORE_TRACE("VulkanAPI created")
}

Brotherhood::VulkanAPI::~VulkanAPI()
{
	vmaDestroyAllocator(m_Allocator);
	vkDestroyDevice(m_Device, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
	BROTHER_CORE_TRACE("VulkanAPI deleted")
}
