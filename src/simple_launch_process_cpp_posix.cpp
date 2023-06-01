#include <signal.h>
#include <assert.h>
#include "simple_launch_process_cpp/simple_launch_process_cpp.h"
#include <condition_variable>
#include <iostream>

namespace simple_launch_process_cpp
{
namespace detail
{

    // Create a class to redirect SIGTERM and SIGINT signals to a callback
    class WaitForExitImpl
    {
    public:

        // Callback function type
        typedef void (*Callback)();
        static int impl_counter;
        static WaitForExitImpl* impl;
        Callback callback;
        std::condition_variable cv;
        bool exit_received = false;
        std::mutex exit_received_mutex;

        // Constructor
        WaitForExitImpl()
        {
            assert(impl_counter == 0);
            impl_counter++;
            impl = this;            
        }

        void SetCallback(Callback callback)
        {
            this->callback = callback;
            // Register signal handlers
            
            signal(SIGTERM, WaitForExitImpl::SignalHandler);
            signal(SIGINT, WaitForExitImpl::SignalHandler);

        }

        // Destructor
        ~WaitForExitImpl()
        {
            // Unregister signal handlers
            signal(SIGTERM, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            impl = NULL;
            impl_counter--;
        }

        // Signal handler
        static void SignalHandler(int signum)
        {
            if (impl != NULL)
                impl->callback();
        }

        static void wait_exit_callback()
        {
            std::unique_lock<std::mutex> lck(impl->exit_received_mutex);
            impl->exit_received = true;
            impl->cv.notify_all();
        }

        void wait_exit()
        {
            SetCallback(wait_exit_callback);
            std::unique_lock<std::mutex> lck(exit_received_mutex);
            while (!exit_received)
                cv.wait(lck);
        }
       
    };

    int WaitForExitImpl::impl_counter = 0;
    WaitForExitImpl* WaitForExitImpl::impl = NULL;

}

// Implement WaitForExit

CWaitForExit::CWaitForExit()
{
    impl = new detail::WaitForExitImpl();

}

CWaitForExit::~CWaitForExit()
{
    delete impl;

}

bool CWaitForExit::CallbackWaitForExit(void (*exit_callback)())
{
    impl->SetCallback(exit_callback);
    return true;
}

void CWaitForExit::WaitForExit()
{
    impl->wait_exit();
}

}
    