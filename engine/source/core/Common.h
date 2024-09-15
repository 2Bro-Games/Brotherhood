#ifndef COMMON_H
#define COMMON_H

#include "core/logger/Log.h"
#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#endif // !COMMON_H
