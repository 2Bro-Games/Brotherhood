#include "RendererAPI.h"
#include "core/logger/Log.h"

#include "vulkan_api/VulkanAPI.h"

#include <exception>

namespace Brotherhood {
	Renderer* Renderer::s_pRenderer = nullptr;
	RendererAPI Renderer::s_API = RendererAPI::NONE;
	Window* Renderer::s_pWindow = nullptr;

	void Renderer::Init(Window* pWindow, const RendererAPI api /* = VULKAN */) {
		s_pWindow = pWindow;
		s_API = api;
		switch (api) {
			case RendererAPI::NONE:
			case RendererAPI::VULKAN: {
				s_pRenderer = new VulkanAPI;
				break;
			}
			default:
				BROTHER_CORE_ERROR("Unknown Render API");
				break;
		}
	}

	void Renderer::Destroy() {
		delete s_pRenderer;
	}
} // Brotherhood