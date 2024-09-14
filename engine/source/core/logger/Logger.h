#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace Brotherhood {
	enum class LogLevel : int {
		trace,
		debug,
		info,
		warn,
		error
	};

	class Logger {
	public:
		Logger(std::string name)
			: m_sName(std::move(name)) {
		}

		void Log(LogLevel level, std::string msg) {
			std::string append = m_sName;
			switch (level)
			{
			case LogLevel::trace:
				append += " [TRACE]";
				break;
			case LogLevel::debug:
				append += " [DEBUG]";
				break;
			case LogLevel::info:
				append += " [INFO]";
				break;
			case LogLevel::warn:
				append += " [WARN]";
				break;
			case LogLevel::error:
				append += " [ERROR]";
				break;
			}

			append += " " + msg + "\n";
			std::printf(append.c_str());
		}

		template<typename T>
		void Trace(const T& msg) {
			Log(LogLevel::trace, msg);
		}

		template<typename T>
		void Debug(const T& msg) {
			Log(LogLevel::debug, msg);
		}

		template<typename T>
		void Info(const T& msg) {
			Log(LogLevel::info, msg);
		}

		template<typename T>
		void Warn(const T& msg) {
			Log(LogLevel::warn, msg);
		}

		template<typename T>
		void Error(const T& msg) {
			Log(LogLevel::error, msg);
		}

	private:
		const std::string m_sName;
	};
}

#endif // !LOGGER_H
