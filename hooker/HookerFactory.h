//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERFACTORY_H
#define C_11_HOOKERFACTORY_H

#include <mutex>
#include <memory>
#include "Hooker.h"
#include "singleton.h"

namespace hooker {
    class HookerFactory {
    public:
        static HookerFactory* getInstance();
		std::unique_ptr<Hooker> getHooker();
    private:
        explicit HookerFactory(){}
		friend class utils::design_pattern::NewPolicy<HookerFactory>;
		friend class utils::design_pattern::Singleton<HookerFactory>;
    };
}


#endif //C_11_HOOKERFACTORY_H
