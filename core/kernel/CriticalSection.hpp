#pragma once
#include <signal.h>

class CriticalSection
{
private:
    CriticalSection() = default;
    ~CriticalSection() = default;
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

public:
    static void enter()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGALRM);
        sigprocmask(SIG_BLOCK, &mask, nullptr);
    }

    static void exit()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGALRM);
        sigprocmask(SIG_UNBLOCK, &mask, nullptr);
    }
};

// RAII
struct ScopedCriticalSection
{
public:
    ScopedCriticalSection()
    {
        CriticalSection::enter();
    }

    ~ScopedCriticalSection()
    {
        CriticalSection::exit();
    }
};