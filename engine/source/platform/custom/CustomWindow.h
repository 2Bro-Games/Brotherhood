#ifndef CUSTOM_WINDOW_H
#define CUSTOM_WINDOW_H

#include <GLFW/glfw3.h>
#include "../Window.h"

namespace Brotherhood {
	class CustomWindow : public Window {
	public:
		CustomWindow(const WindowStruct& windowStruct);
		virtual ~CustomWindow();

		virtual void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_WindowStruct.m_nWidth; }
		inline unsigned int GetHeight() const override { return m_WindowStruct.m_nHeight; }

		inline void SetEventCallback(const EventCallbackFn& callback) override {
			m_WindowStruct.m_EventCallback = callback;
		}

		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

	private:
		virtual void Init(const WindowStruct& windowStruct);
		virtual void Shutdown();

	private:
		GLFWwindow* m_pWindow;
	};
}

#endif // !CUSTOM_WINDOW_H
