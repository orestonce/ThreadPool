#include <iostream>
#include "ThreadPool.h"
#include <chrono>
#include <random>

std::random_device rd;
std::mutex io_mutex;
std::atomic_int i(0);

class MyJob
        : public restonce::ThreadJob
{
protected:
    virtual void run() noexcept override final
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(rd() % 1000 ));
        {
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout << "Hello world : " << i << std::endl;
            ++i;
        }
    }
};

int main()
{
    restonce::ThreadPool pool(100);

    for(int i=0; i<600; ++i) {
        pool.submit (std::shared_ptr<restonce::ThreadJob>( new MyJob()));
//        if ( (i+1) % 200 ==0 ) {
//            std::cout << "waitAll" << std::endl;
//            pool.waitAll ();
//        }
    }

    pool.waitAll ();
    std::cout << "End !" << std::endl;

    return 0;
}

