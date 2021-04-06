#pragma once

#include <deque>

#include <boost/noncopyable.hpp>

#include <boost/assert.hpp>
#include <boost/concept_check.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

template <typename Job>
class job_queue : boost::noncopyable
{
public:
    typedef Job job_type;
    typedef std::deque<job_type> queue_type;

    typedef boost::mutex mutex_type;
    typedef boost::unique_lock<mutex_type> lock_type;
    typedef boost::condition_variable_any condition_type;

    BOOST_CONCEPT_ASSERT((boost::SGIAssignable<job_type>));
    BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<job_type>));

    job_queue() : m_stop_flag(false)
    {
    }

    void push(const job_type &x)
    {
        lock_type lock(m_mutex);
        m_queue.push_back(x);
        m_hasJob.notify_one();
    }

    job_type pop()
    {
        lock_type lock(m_mutex);
        while (m_queue.empty() && !m_stop_flag)
        {
            m_hasJob.wait(m_mutex);
        }

        if (m_stop_flag)
        {
            return job_type();
        }

        BOOST_ASSERT(!m_queue.empty());

        job_type tmp = m_queue.front();
        m_queue.pop_front();

        return tmp;
    }

    void stop()
    {
        m_stop_flag = true;
        m_hasJob.notify_all();
    }

private:
    queue_type m_queue;

    mutex_type m_mutex;
    condition_type m_hasJob;

    bool m_stop_flag;
};
