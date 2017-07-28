//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERTHUMB_H
#define C_11_HOOKERTHUMB_H

#include "Hooker.h"

namespace hooker {
    class HookerThumb : public Hooker {
    public:
        void doHook(void *func, void *newAddr, void **origFunc);
        size_t getHookHeadSize() {
            return 12;
        }
		size_t getOrigFunctionSize() {
			return 2 * getHookHeadSize();
		}
    };
}

#endif //C_11_HOOKERTHUMB_H
