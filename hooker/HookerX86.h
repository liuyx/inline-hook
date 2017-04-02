//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERX86_H
#define C_11_HOOKERX86_H


#include "Hooker.h"

namespace hooker {
    class HookerX86 : public Hooker {
    public:
        void doHook(void *func, void *newAddr, void **origFunc);
        size_t getHookHeadSize() {
            return 6;
        }
    };
}


#endif //C_11_HOOKERX86_H
