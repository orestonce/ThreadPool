#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <thread>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <vector>
#include <tuple>
#include <atomic>

namespace restonce
{
class ThreadJob
{
public :
    friend class ThreadPool;
    ThreadJob() = default;
    virtual ~ThreadJob() = default;
protected:
    virtual void run() noexcept = 0;
};
class ThreadPool
{
public:
    enum {
        DEFAULT_THREAD_COUNT = 4
    };
    ThreadPool(int threadCount = DEFAULT_THREAD_COUNT );
    void submit( std::shared_ptr<ThreadJob> newJob);
    void waitAll();
    virtual ~ThreadPool();
private :
    void threadRun();
private:
    std::vector<std::thread> threads;
    std::condition_variable conditon_variable_newjob;
    std::condition_variable conditon_variable_empty;
    std::atomic_bool quit;
    std::deque< std::shared_ptr<ThreadJob> > jobs;
    std::mutex mutex_job;
    std::atomic_int restJobCount;
};
}

#endif // THREADPOOL_H
