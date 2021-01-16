#pragma once
#include <assert.h>
#include <memory>

template <typename T>
class s
{
public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }
protected:
    s() {}
    ~s() {}
public:
    s(s const&) = delete;
    s& operator=(s const&) = delete;
};
