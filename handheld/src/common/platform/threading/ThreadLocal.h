#pragma once
#include "Core/Debug/Log.h"
//Recreated

template<typename T>
class ThreadLocal
{
public:
    ThreadLocal() = default;

    template<typename F>
    ThreadLocal(F&& func)
        : mCreate(std::forward<F>(func))
    {
    }

    T& getLocal()
    {
        DEBUG_ASSERT(mCreate, "Invalid create function");

        thread_local std::unordered_map<
            const ThreadLocal<T>*,
            std::unique_ptr<T>
        > locals;

        auto& local = locals[this];

        if (!local)
            local = mCreate();

        return *local;
    }

private:
    std::function<std::unique_ptr<T>()> mCreate;
};