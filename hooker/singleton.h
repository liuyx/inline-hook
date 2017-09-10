#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#include <mutex>
#include <cstdlib>

namespace utils {
namespace design_pattern{

template <typename T>
struct SingleThreadModel {
	typedef T InstanceType;
	typedef int Lock;
};

template <typename T>
struct NewPolicy {
	static T* create() {
		return new T;
	}
};

template <typename T>
struct DeletePolicy {
	static void destroy(T *ptr) {
		delete ptr;
	}
};

template <typename T>
struct FreePolicy {
	static void destroy(T *ptr) {
		delete ptr;
	}
};

template <
	typename T,
	template <typename> class ThreadingModel = SingleThreadModel,
	template <typename> class CreatePolicy = NewPolicy,
	template <typename> class DestroyPolicy = DeletePolicy
>
class Singleton {
public:
	static T* getInstance();
	void destroy() {
		DestroyPolicy<T>::destroy(pInstance_);
	}
private:
	Singleton();
	Singleton(const Singleton& );
	Singleton& operator=(const Singleton&);

	typedef typename ThreadingModel<T>::InstanceType InstanceType;
	static InstanceType *pInstance_;
};

template <typename T,template <typename> class ThreadingModel, template <typename> class CreatePolicy, template <typename> class DestroyPolicy>
typename Singleton<T,ThreadingModel,CreatePolicy,DestroyPolicy>::InstanceType *
Singleton<T,ThreadingModel,CreatePolicy,DestroyPolicy>:: pInstance_ = nullptr;

template <
	typename T,
	template <typename> class ThreadingModel,
	template <typename> class CreatePolicy,
	template <typename> class DestroyPolicy>
T* Singleton<T,ThreadingModel,CreatePolicy, DestroyPolicy>::getInstance() {
	if (!pInstance_) {
		typename ThreadingModel<T>::Lock lock;
		pInstance_ = CreatePolicy<T>::create();
	}
	return const_cast<T*>(pInstance_);

}

class Uncopable {
public:
	Uncopable() {}
	~Uncopable() {}
private:
	Uncopable(const Uncopable&);
	Uncopable& operator = (const Uncopable&);
};

template <typename T>
struct MultiThreadPolicy : private Uncopable {
	typedef T volatile InstanceType;
	static std::mutex sMutex;
	struct Lock {
		Lock() {
			sMutex.lock();
		}
		~Lock() {
			sMutex.unlock();
		}
	};
};

template <typename T>
std::mutex MultiThreadPolicy<T>::sMutex;


template <typename T>
struct MallocPolicy {
	static T* Create() {
		return (T *)malloc(sizeof(T));
	}
};

} // namespace design_pattern
} // namespace utils

#endif // _SINGLETON_H_
