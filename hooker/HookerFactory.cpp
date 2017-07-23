//
// Created by lamoliu on 2017/4/1.
//

#include <utility>
#include <memory>
#include <mutex>
#include <HookerFactory.h>
#include <config.h>
#include <HookerError.h>

hooker::HookerFactory* hooker::HookerFactory::getInstance() {
	using namespace utils::design_pattern;
	return Singleton<HookerFactory,MultiThreadPolicy>::getInstance();
}

std::unique_ptr<hooker::Hooker> hooker::HookerFactory::getHooker() {
#ifdef __x86_64__
    return std::unique_ptr<hooker::Hooker>(std::move(new hooker::HookerX64));
#elif defined(__i386__)
    return std::unique_ptr<hooker::Hooker>(std::move(new hooker::HookerX86));
#elif defined(__arm__)
    return std::unique_ptr<hooker::Hooker>(std::move(new hooker::HookerArm));
#elif defined(__thumb__)
    return std::unique_ptr<hooker::Hooker>(std::move(new hooker::HookerThumb));
#else
	throw hooker::error::HookerError("unsupported hook architecture");
#endif

}

