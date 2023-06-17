
#ifndef DREKAR_LAUNCH_PROCESS_CPP_H
#define DREKAR_LAUNCH_PROCESS_CPP_H

#pragma once

namespace drekar_launch_process_cpp
{
    namespace detail
    {
        class WaitForExitImpl;
    }
    class CWaitForExit
    {
        detail::WaitForExitImpl* impl;
    public:
        CWaitForExit();
        ~CWaitForExit();

        void WaitForExit();
        bool CallbackWaitForExit(void (*exit_callback)());
    };
}

#endif // DREKAR_LAUNCH_PROCESS_CPP_H