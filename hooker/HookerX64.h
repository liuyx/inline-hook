//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERX64_H
#define C_11_HOOKERX64_H

#include "Hooker.h"

namespace hooker {
class HookerX64 : public Hooker {
public:
    void doHook(void *func, void *newAddr, void **origFunc) const;
    size_t getHookHeadSize() const {
        return 14;
    }
	~HookerX64() {}
};
}

#endif //C_11_HOOKERX64_H
