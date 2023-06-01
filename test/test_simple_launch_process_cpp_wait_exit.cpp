#include "simple_launch_process_cpp/simple_launch_process_cpp.h"
#include <iostream>

int main()
{
    simple_launch_process_cpp::CWaitForExit wait_for_exit;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    wait_for_exit.WaitForExit();
    std::cout << "Exit" << std::endl;
    return 0;
}