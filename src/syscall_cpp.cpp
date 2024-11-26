#include "../h/syscall_cpp.hpp"


Semaphore::Semaphore(unsigned int init) {
    sem_open(&this->myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal(this->myHandle);
}

int Semaphore::tryWait() {
    return sem_trywait(this->myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread(void (*body)(void *), void *arg):  myHandle(nullptr), body(body), arg(arg) {}

void Thread::join() {
    thread_join(myHandle);
}

Thread::~Thread() {
    //join();
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

Thread::Thread(): myHandle(nullptr), body(runWrapper), arg(this) {}

void Thread::run() {
    //This is empty because it is supposed to be overriden
}

int Thread::sleep(time_t time)
{
    return time_sleep(time);
}
void Console::putc(char c)
{
    ::putc(c);
}

char Console::getc()
{
    return ::getc();
}