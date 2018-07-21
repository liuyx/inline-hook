//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKER_H
#define C_11_HOOKER_H

#include <cstdint>
#include <unordered_map>

namespace hooker {
#define PAGE_START(x,pagesize)	((x) &~ ((pagesize) - 1))
#define CODE_WRITE				PROT_READ | PROT_WRITE | PROT_EXEC
#define CODE_READ_ONLY			PROT_READ | PROT_EXEC

    class Hooker {
    public:
        void changeCodeAttrs(void *func, int attr) const;
        void hook(void *func, void *newAddr, void **origFunc, bool saveOrig = true) const;
        void unhook(void *func, void *oldFunc = nullptr) const;
        virtual size_t getHookHeadSize() const = 0;

		// when construct the origin function, it's best to know
		// how many bytes should we allocate, so when we just use
		// the origin function as normal.
		virtual size_t getOrigFunctionSize() const {
			return getHookHeadSize();
		}

		virtual ~Hooker() = 0;

    protected:
        virtual void doHook(void *func, void *newAddr, void **origFunc) const = 0;
        virtual void doUnHook(void *func, void *oldFunc = nullptr) const;
        mutable std::unordered_map<long,long> gHookedMap;
    private:
        void saveOriginFuncBytes(void *func) const;
    };
}


#endif //C_11_HOOKER_H
