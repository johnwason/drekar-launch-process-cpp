#include "drekar_launch_process_cpp/drekar_launch_process_cpp.h"
#include <iostream>
#include <condition_variable>
#include <functional>

std::condition_variable cv;
std::mutex cv_mutex;
bool exit_received = false;

void exit_callback()
{
    std::unique_lock<std::mutex> lock(cv_mutex);
    exit_received = true;
    cv.notify_all();
}

int main()
{
    drekar_launch_process_cpp::CWaitForExit wait_for_exit;
    wait_for_exit.CallbackWaitForExit(exit_callback);
    std::unique_lock<std::mutex> lock(cv_mutex);
    std::cout << "Press Ctrl+C to exit" << std::endl;
    cv.wait(lock, [] {return exit_received; });
    std::cout << "Exit" << std::endl;
    return 0;
}