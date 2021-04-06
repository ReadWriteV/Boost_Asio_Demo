#pragma once

#include <boost/assert.hpp>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

template <typename Queue>
class worker
{
public:
    typedef Queue queue_type;
    typedef typename Queue::job_type job_type;
    typedef boost::function<bool(job_type &)> func_type;

    template <typename Func>
    worker(queue_type &q, Func func, int n = 1) : m_queue(q), m_func(func), m_thread_num(n)
    {
        BOOST_ASSERT(n > 0);
    }

    worker(queue_type &q, int n = 1) : m_queue(q), m_thread_num(n)
    {
        BOOST_ASSERT(n > 0);
    }

    void start()
    {
        BOOST_ASSERT(m_func);
        if (m_threads.size() > 0)
        {
            return;
        }
        for (int i = 0; i < m_thread_num; ++i)
        {
            m_threads.create_thread(boost::bind(&worker::do_work, this));
        }
    }

    template <typename Func>
    void start(Func func)
    {
        m_func = func;
        start();
    }

    void run()
    {
        start();
        m_threads.join_all();
    }

    void stop()
    {
        m_func = 0;
        m_queue.stop();
    }

private:
    void do_work()
    {
        for (;;)
        {
            job_type job = m_queue.pop();
            if (!m_func || !m_func(job))
            {
                break;
            }
        }
    }

    queue_type &m_queue;
    func_type m_func;

    int m_thread_num;
    boost::thread_group m_threads;
};