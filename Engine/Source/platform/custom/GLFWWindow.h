#ifndef CUSTOM_WINDOW_H
#define CUSTOM_WINDOW_H

#include "../Window.h"
#include <GLFW/glfw3.h>

namespace Brotherhood {
	class GLFWWindow final : public Window {
	public:
		GLFWWindow(const WindowStruct& windowStruct);

		virtual ~GLFWWindow() override;

		virtual void OnUpdate() override;

		inline unsigned int GetWidth() const override {
			return m_WindowStruct.m_nWidth;
		}
		inline unsigned int GetHeight() const override {
			return m_WindowStruct.m_nHeight;
		}

		inline void SetEventCallback(const EventCallbackFn& callback) override {
			m_WindowStruct.m_EventCallback = callback;
		}

		virtual void SetVSync(const bool enabled) override {
			m_WindowStruct.m_bVSync = enabled;
		}
		virtual bool IsVSync() const override {
			return m_WindowStruct.m_bVSync;
		}

	private:
		virtual void Init(const WindowStruct& windowStruct);

		virtual void Shutdown();

	private:
		GLFWwindow* m_pWindow;
	};
}

#endif // !CUSTOM_WINDOW_H
