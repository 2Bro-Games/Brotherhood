#include "RendererAPI.h"
#include "Core/Logger/Log.h"

std::unique_ptr<Brotherhood::RendererAPI> Brotherhood::RendererAPI::Create(Brotherhood::RenderAPI api)
{
	switch (api)
	{
	case Brotherhood::NONE:
	case Brotherhood::VULKAN:
		BROTHER_CORE_TRACE("VULKAN RenderAPI Created");
		break;
	default:
		BROTHER_CORE_ERROR("Unknown Render API");
		break;
	}

	static_assert(1!=0);
	return std::unique_ptr<RendererAPI>(nullptr);
}
