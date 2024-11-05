#ifndef WINDOW_H
#define WINDOW_H

#include "core/event/Event.h"
#include <functional>
#include <string>
#include <utility>

struct GLFWwindow;

namespace Brotherhood {
	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(const bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		struct WindowStruct {
			std::string m_sTitle;
			unsigned int m_nWidth;
			unsigned int m_nHeight;
			bool m_bVSync;

			WindowStruct(
				std::string title = "Brother",
				const unsigned int width = 1280,
				const unsigned int height = 720)
				: m_sTitle(std::move(title))
				, m_nWidth(width)
				, m_nHeight(height)
				, m_bVSync(false) {
			}

			EventCallbackFn m_EventCallback;
		};

		static Window* Create(const WindowStruct& windowStruct = WindowStruct());
		static void Destroy(Window* pWindow);

	protected:
		WindowStruct m_WindowStruct;
	};
}

#endif