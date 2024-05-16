#ifndef THREADS_H
#define THREADS_H

#ifdef __cplusplus
extern "C" {
#endif



#include "../../Build-Info.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS)
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
typedef pthread_t Thread;
typedef sem_t* Semaphore;
#endif

#if defined(SYSTEM_WINDOWS)
#include <Windows.h>
#include <processthreadsapi.h>
typedef HANDLE Thread;
typedef HANDLE Semaphore;
#endif

void create_thread(Thread* in_Thread, void*(*in_Callback)(void*), void* in_Arguments);
void await_thread(Thread in_Thread);
void create_semaphore(Semaphore* in_Semaphore, const char* in_Name);
void destroy_semaphore(Semaphore in_Semaphore, const char* in_Name);
void wait_semaphore(Semaphore in_Semaphore);
void signal_semaphore(Semaphore in_Semaphore);



#ifdef __cplusplus
}
#endif

#endif
