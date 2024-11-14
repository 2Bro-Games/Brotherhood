#ifndef COMMON_H
#define COMMON_H

#include "core/logger/Log.h"

#include <format>
#include <cassert>
#include <unordered_map>

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn)                                       \
	[this](auto&&... args) -> decltype(auto) {                  \
		return this->fn(std::forward<decltype(args)>(args)...); \
	}

namespace Utils {
	inline void Check(const bool result, const std::string&& sMessage) {
		if (result) {
			BROTHER_CORE_ERROR(sMessage);
			assert(result);
		}
	}
}
#endif // !COMMON_H
