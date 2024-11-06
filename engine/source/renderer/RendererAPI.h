#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace Brotherhood {
	class Window;

	enum class RendererAPI {
		NONE = 0,
		VULKAN
	};

	class Renderer {
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		static void Init(Window* pWindow, const RendererAPI api = RendererAPI::VULKAN);
		static void Destroy();

		static RendererAPI GetAPI() {
			return s_API;
		};

	protected:
		static RendererAPI s_API;
		static Renderer* s_pRenderer;
		static Window* s_pWindow;
	};
} // Brotherhood

#endif // !RENDERER_API_H
