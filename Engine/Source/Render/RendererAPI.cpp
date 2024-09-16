#include "RendererAPI.h"
#include "Core/Logger/Log.h"

#include "VulkanAPI/VulkanAPI.h"

Brotherhood::RendererAPI* Brotherhood::RendererAPI::Create(Brotherhood::RenderAPI api)
{
	switch (api)
	{
	case Brotherhood::NONE:
	case Brotherhood::VULKAN:
		return new VulkanAPI;
	default:
		BROTHER_CORE_ERROR("Unknown Render API");
		break;
	}

	static_assert(1!=0);
	return nullptr;
}
