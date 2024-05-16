#include "Threads.h"

#if defined(SYSTEM_WINDOWS)



void create_thread(Thread* in_Thread, void*(*in_Callback)(void*), void* in_Arguments) {
	*in_Thread = CreateThread(NULL, 0, (DWORD(*)(LPVOID))in_Callback, (LPVOID)in_Arguments, 0, NULL);
}
void await_thread(Thread in_Thread) {
	WaitForSingleObject(in_Thread, INFINITE);
	CloseHandle(in_Thread);
}
void create_semaphore(Semaphore* in_Semaphore, const char* in_Name) {
	*in_Semaphore = CreateSemaphoreA(NULL, 1, 1, in_Name);
}
void destroy_semaphore(Semaphore in_Semaphore, const char* in_Name) {
	CloseHandle(in_Semaphore);
}
void wait_semaphore(Semaphore in_Semaphore) {
	WaitForSingleObject(in_Semaphore, INFINITE);
}
void signal_semaphore(Semaphore in_Semaphore) {
	ReleaseSemaphore(in_Semaphore, 1, NULL);
}



#endif
