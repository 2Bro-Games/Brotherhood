#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <vulkan/vulkan.h>
#include <exception>
#include <memory>

namespace Brotherhood {
	enum RenderAPI
	{
		NONE = 0,
		VULKAN
	};

	class RendererAPI {
	public:
		static RendererAPI* Create(RenderAPI api);

		RendererAPI() = default;
		virtual ~RendererAPI() = default;
	};
}

#endif // !RENDERER_API_H
