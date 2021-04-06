#pragma once

#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>

template <typename T>
class safe_singleton : boost::noncopyable
{
public:
    typedef T real_type;

    static real_type &instance()
    {
        boost::call_once(m_flag, init);
        return init;
    }

private:
    static real_type &init()
    {
        static real_type obj;
        return obj;
    }
    static boost::once_flag m_flag;
};

template <typename T>
boost::once_flag safe_singleton<T>::m_flag;