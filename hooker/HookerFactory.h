//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERFACTORY_H
#define C_11_HOOKERFACTORY_H

#include <mutex>
#include <memory>
#include "Hooker.h"

namespace hooker {
    class HookerFactory {
    public:
        static std::unique_ptr<HookerFactory> getInstance();
		std::unique_ptr<Hooker> getHooker();
    private:
        explicit HookerFactory(){}
    };
}


#endif //C_11_HOOKERFACTORY_H
