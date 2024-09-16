#include "VulkanAPI.h"
#include "Core/Logger/Log.h"

Brotherhood::VulkanAPI::VulkanAPI()
{
	CreateInstance();
	PickPhysicalDevice();
	CreateDevice();
	BROTHER_CORE_TRACE("VulkanAPI created")
}

Brotherhood::VulkanAPI::~VulkanAPI()
{
	vkDestroyDevice(m_Device, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
	BROTHER_CORE_TRACE("VulkanAPI deleted")
}
