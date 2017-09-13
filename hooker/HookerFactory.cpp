//
// Created by lamoliu on 2017/4/1.
//

#include <utility>
#include <memory>
#include <mutex>
#include <HookerFactory.h>
#include <config.h>
#include <HookerError.h>

std::unique_ptr<hooker::HookerFactory> hooker::HookerFactory::getInstance() {
	using namespace utils::design_pattern;
	HookerFactory* result =  Singleton<HookerFactory,MultiThreadPolicy>::getInstance();
	return std::unique_ptr<HookerFactory>(result);
}

hooker::Hooker* hooker::HookerFactory::createHooker() {
#ifdef __x86_64__
    return new hooker::HookerX64;
#elif defined(__i386__)
    return new hooker::HookerX86;
#elif defined(__arm__)
    return new hooker::HookerArm;
#elif defined(__thumb__)
    return new hooker::HookerThumb;
#else
	throw hooker::error::HookerError("unsupported hook architecture");
#endif
}

const hooker::Hooker& hooker::HookerFactory::getHooker() const {
	return *hooker;
}

