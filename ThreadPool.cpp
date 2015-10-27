#include "ThreadPool.h"
#include <iostream>

namespace restonce
{
ThreadPool::ThreadPool(int threadCount)
    : quit(false)
{
    restJobCount = 0;
    for(int i=0; i<threadCount; ++i)
    {
        threads.push_back (std::thread(&ThreadPool::threadRun,this));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mutex_job);
        quit = true;
        jobs.clear ();
    }
    conditon_variable_newjob.notify_all ();
    for(auto& t : threads)
    {
        t.join();
    }
}

void ThreadPool::threadRun()
{
    while ( true )
    {
        std::unique_lock<std::mutex> lock(mutex_job);

        conditon_variable_newjob.wait (lock, [&]() -> bool {
                                    return quit || !jobs.empty ();
                                });
        if ( !jobs.empty () ) {
            std::shared_ptr<ThreadJob> myjob = jobs.front ();
            jobs.pop_front ();

            lock.unlock ();
            myjob->run ();
            -- restJobCount;
            if ( restJobCount <= 0 ) {
                conditon_variable_empty.notify_all ();
            }
//            std::cout << restJobCount << std::endl;
        } else {
            break;
        }
    }
}

void ThreadPool::submit(std::shared_ptr<ThreadJob> newJob)
{
    std::unique_lock<std::mutex> lock(mutex_job);
    ++ restJobCount;
    jobs.push_back (newJob);
    conditon_variable_newjob.notify_one ();
}

void ThreadPool::waitAll()
{
    std::unique_lock<std::mutex> lock(mutex_job);
    conditon_variable_empty.wait (lock, [&]() -> bool{
                                      return jobs.empty () && restJobCount == 0 ;
                                  } );
}

}
