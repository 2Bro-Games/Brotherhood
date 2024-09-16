#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <vulkan/vulkan.h>
#include <memory>

namespace Brotherhood {
	enum RenderAPI
	{
		NONE = 0,
		VULKAN
	};

	class RendererAPI {
		friend class std::unique_ptr<RendererAPI>;

	public:
		static std::unique_ptr<RendererAPI> Create(RenderAPI api);

		RendererAPI() = delete;
		virtual ~RendererAPI() = default;
	};
}

#endif // !RENDERER_API_H
