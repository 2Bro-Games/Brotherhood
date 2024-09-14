#ifndef LOG_H
#define LOG_H

#include <memory>
#include "Logger.h"

namespace Brotherhood {
	class Log {
	public:
		static void Init();

		static std::shared_ptr<Logger> GetCoreLogger() { return m_pCoreLogger; };
		static std::shared_ptr<Logger> GetClientLogger() { return m_pClientLogger; };

	private:
		static std::shared_ptr<Logger> m_pCoreLogger;
		static std::shared_ptr<Logger> m_pClientLogger;
	};
}

// Core log macros
#define BROTHER_CORE_TRACE(...)    ::Brotherhood::Log::GetCoreLogger()->Trace(__VA_ARGS__);
#define BROTHER_CORE_INFO(...)     ::Brotherhood::Log::GetCoreLogger()->Info(__VA_ARGS__);
#define BROTHER_CORE_WARN(...)     ::Brotherhood::Log::GetCoreLogger()->Warn(__VA_ARGS__);
#define BROTHER_CORE_ERROR(...)    ::Brotherhood::Log::GetCoreLogger()->Error(__VA_ARGS__);

// Client log macros
#define BROTHER_TRACE(...)         ::Brotherhood::Log::GetClientLogger()->Trace(__VA_ARGS__);
#define BROTHER_INFO(...)          ::Brotherhood::Log::GetClientLogger()->Info(__VA_ARGS__);
#define BROTHER_WARN(...)          ::Brotherhood::Log::GetClientLogger()->Warn(__VA_ARGS__);
#define BROTHER_ERROR(...)         ::Brotherhood::Log::GetClientLogger()->Error(__VA_ARGS__);

#endif // !LOG_H
