#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <string>

namespace Brotherhood {
	class Window {
	public:
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;
		virtual bool ShouldClose() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		struct WindowStruct {
			std::string m_sTitle;
			unsigned int m_nWidth;
			unsigned int m_nHeight;
			bool m_bVSync;

			WindowStruct(const std::string& title = "Victory",
				unsigned int width = 1280, unsigned int height = 720)
				: m_sTitle(title), m_nWidth(width)
				, m_nHeight(height), m_bVSync(false) {}
		};

		static Window* Create(const WindowStruct& windowStruct = WindowStruct());

	protected:
		WindowStruct m_WindowStruct;
	};
}

#endif