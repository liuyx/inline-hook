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
        static std::unique_ptr<HookerFactory> getInstance();
		const Hooker& getHooker() const;
		Hooker* createHooker();
		~HookerFactory() {
			delete hooker;
		}
    private:
		Hooker* hooker;
        HookerFactory(){
			hooker = createHooker();
		}
		friend class utils::design_pattern::NewPolicy<HookerFactory>;
		friend class utils::design_pattern::Singleton<HookerFactory>;
    };
}


#endif //C_11_HOOKERFACTORY_H
