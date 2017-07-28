//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERARM_H
#define C_11_HOOKERARM_H


#include "Hooker.h"

namespace hooker {
    class HookerArm : public Hooker {
    public:
        void doHook(void *func, void *newAddr, void **origFunc);
        size_t getHookHeadSize() {
            return 8;
        }
		size_t getOrigFunctionSize() {
			return 2 * getHookHeadSize();
		}
    };
}


#endif //C_11_HOOKERARM_H
