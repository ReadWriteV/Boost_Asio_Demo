#pragma once

#include <vector>

#include <boost/bind/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

class scheduler
{
public:
    typedef boost::function<void()> func_type;
    typedef std::pair<func_type, int> schedule_type;
    typedef std::vector<schedule_type> schedule_funcs;

    void add(func_type func, int seconds)
    {
        m_funcs.push_back(std::make_pair(func, seconds));
    }

    void start()
    {
        for (schedule_funcs::reference x : m_funcs)
        {
            m_threads.create_thread(boost::bind(&scheduler::schedule, this, x.first, x.second));
        }
    }

    void run()
    {
        start();
        m_threads.join_all();
    }

private:
    void schedule(func_type func, int sec)
    {
        for (;;)
        {
            func();
            boost::this_thread::sleep(boost::posix_time::seconds(sec));
        }
    }

    schedule_funcs m_funcs;
    boost::thread_group m_threads;
};