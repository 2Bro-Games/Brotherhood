#include "Log.h"

std::shared_ptr<Brotherhood::Logger> Brotherhood::Log::m_pClientLogger;
std::shared_ptr<Brotherhood::Logger> Brotherhood::Log::m_pCoreLogger;

void Brotherhood::Log::Init() {
	Log::m_pClientLogger = std::make_shared<Logger>("CLIENT");
	Log::m_pCoreLogger = std::make_shared<Logger>("BROTHERHOOD");
}