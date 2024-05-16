#include "Threads.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS)



void create_thread(Thread* in_Thread, void*(*in_Callback)(void*), void* in_Arguments) {
	pthread_create(in_Thread, NULL, in_Callback, in_Arguments);
}
void await_thread(Thread in_Thread) {
	pthread_join(in_Thread, NULL);
}
void create_semaphore(Semaphore* in_Semaphore, const char* in_Name) {
	*in_Semaphore = sem_open(in_Name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
}
void destroy_semaphore(Semaphore in_Semaphore, const char* in_Name) {
	sem_unlink(in_Name);
}
void wait_semaphore(Semaphore in_Semaphore) {
	sem_wait(in_Semaphore);
}
void signal_semaphore(Semaphore in_Semaphore) {
	sem_post(in_Semaphore);
}



#endif
