#pragma once
#include "Exception.hpp"
#include "KernelContext.hpp"

class SyscallHandler
{
public:
    SyscallHandler(SystemContext* ctx, StorageContext* storage);
    SyscallStatus dispatch(SyscallID id);

private:
    SystemContext* systemCtx;
    StorageContext* storageCtx;

    // process related
    void handleExit(SyscallStatus& status);
    void handleWrite(SyscallStatus& status);
    void handleRead(SyscallStatus& status);
    void handleOpen(SyscallStatus& status);
    void handleClose(SyscallStatus& status);
    void handleCreate(SyscallStatus& status);
    void handleSbrk(SyscallStatus& status);

    // threads related
    void handleThreadCreate(SyscallStatus& status);
    void handleThreadExit(SyscallStatus& status);
    void handleThreadJoin(SyscallStatus& status);

    // mutex related
    void handleMutexCreate(SyscallStatus& status);
    void handleMutexLock(SyscallStatus& status);
    void handleMutexUnlock(SyscallStatus& status);
};