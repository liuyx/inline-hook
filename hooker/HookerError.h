#ifndef _HOOK_ERROR_H_
#define _HOOK_ERROR_H_

#include <stdexcept>
#include <errno.h>
#include <string>
#include <cstring>

namespace hooker {
	namespace error {
		class HookerError : public std::logic_error {
		public:
			HookerError(std::string msg) : std::logic_error(errno != 0 ? std::string(msg) + ": " + strerror(errno) : msg) {}
		};
	}
}

#endif
