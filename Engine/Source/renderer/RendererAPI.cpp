#include "RendererAPI.h"
#include "core/logger/Log.h"

#include "vulkan_api/VulkanAPI.h"

#include <exception>

namespace Brotherhood {
	static Renderer* s_pRenderer = nullptr;

	void Renderer::Create(const RendererAPI api /* = VULKAN */) {
		switch (api) {
			case RendererAPI::NONE:
			case RendererAPI::VULKAN:
				s_pRenderer = new VulkanAPI;
			default:
				BROTHER_CORE_ERROR("Unknown Render API");
				break;
		}
	}

	void Renderer::Destroy() {
		delete s_pRenderer;
	}
} // Brotherhood