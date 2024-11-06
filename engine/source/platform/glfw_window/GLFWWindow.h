#ifndef CUSTOM_WINDOW_H
#define CUSTOM_WINDOW_H

#include "../Window.h"

#include <GLFW/glfw3.h>

namespace Brotherhood {
	class GLFWWindow final : public Window {
	public:
		GLFWWindow(const WindowStruct& inWindowStruct);
		virtual ~GLFWWindow() override;

		virtual void OnUpdate() override;

		inline uint32_t GetWidth() const override {
			return m_WindowStruct.m_nWidth;
		}
		inline uint32_t GetHeight() const override {
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

		virtual void* GetNativeWindow() override {
			return m_pWindow;
		};

	private:
		GLFWwindow* m_pWindow { nullptr };
	};
}
#endif // !CUSTOM_WINDOW_H
