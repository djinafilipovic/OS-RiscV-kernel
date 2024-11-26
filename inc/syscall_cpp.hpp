#ifndef SYSCALL_CPP_HPP
#define SYSCALL_CPP_HPP

#include "syscall_c.h"

class Thread {
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    void join();

    static void dispatch();
    static int sleep(time_t time);

protected:
    Thread();
    virtual void run();

private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    //wrapper za poziv run() metode
    static void runWrapper(void* thread) {
        if(thread) {
            ((Thread*)thread)->run();
        }
    }
};


class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();
    int tryWait();

private:
    sem_t myHandle;
};

class Console
{
public:
    static char getc();
    static void putc(char c);
};

#endif //SYSCALL_CPP_HPP