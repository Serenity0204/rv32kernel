#ifndef SYSCALL_H
#define SYSCALL_H

// System Call IDs
#define SYS_EXIT 0
#define SYS_WRITE 1
#define SYS_READ 2

#define SYS_THREAD_CREATE 3
#define SYS_THREAD_EXIT 4
#define SYS_THREAD_JOIN 5

#define SYS_MUTEX_CREATE 6
#define SYS_MUTEX_LOCK 7
#define SYS_MUTEX_UNLOCK 8

// mutex id
typedef int mutex_t;

void exit(int code);
int write(int fd, void* buf, int count);
int read(int fd, void* buf, int count);

// threads related syscall
int thread_create(void (*func)(void*), void* arg);
void thread_exit(int status);
int thread_join(int tid);

mutex_t mutex_create();
void mutex_lock(mutex_t* lock);
void mutex_unlock(mutex_t* lock);

#endif