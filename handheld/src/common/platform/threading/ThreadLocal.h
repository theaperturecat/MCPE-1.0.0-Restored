#pragma once
#include "Core/Debug/Log.h"
//Recreated

template<typename T>
class ThreadLocal
{
public:
    ThreadLocal()
    {
    }

    template<typename F>
    ThreadLocal(F&& func)
        : mCreate(std::forward<F>(func))
    {
    }

    T& getLocal()
    {
        DEBUG_ASSERT(mCreate, "Invalid create function");
        thread_local std::unique_ptr<T> local = mCreate();
        return *local;
    }



    std::function<std::unique_ptr<T>()> mCreate;
};