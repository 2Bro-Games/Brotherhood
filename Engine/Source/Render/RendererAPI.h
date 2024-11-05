#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <vulkan/vulkan.h>
#include <memory>

namespace Brotherhood {
	enum class RendererAPI {
		NONE = 0,
		VULKAN
	};

	class Renderer {
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		static void Create(const RendererAPI api = RendererAPI::VULKAN);
		static void Destroy();
	};
} // Brotherhood

#endif // !RENDERER_API_H
