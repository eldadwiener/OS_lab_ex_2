#ifndef _MPI_MESSAGES_API
#define _MPI_MESSAGES_API

#include <sys/types.h>
#include <unistd.h>


typedef struct msgObj
{
    pid_t pid;
    int rank;
    const char* message;
    ssize_t message_size;
}msgObj;

// Description: Send the message, 'message', of size 'message_size', to rank
// Return value: Success - 0, Failure - -1
int send_mpi_message(int rank, const char* message, ssize_t message_size)
{
    msgObj msg = {getpid(),rank,message,message_size};
    int res;
    __asm__ (
        "pushl %%eax;"
        "pushl %%ebx;"
        "pushl %%ecx;"
        "pushl %%edx;"
        "movl $244, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        "movl %%eax,%0;"
        "popl %%edx;"
        "popl %%ecx;"
        "popl %%ebx;"
        "popl %%eax;"
        : "=m" (res)
        : "m"(msg)
    );
    // TODO: Handle post return 
    /*if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    } */
    return (int) res;
}

// Description: Register to the MPI communication list.
// Return value: Success - Rank of process, Failure - -1
int register_mpi()
{
    int res;
    pid_t pid = getpid();
    __asm__ (
        "pushl %%eax;"
        "pushl %%ebx;"
        "pushl %%ecx;"
        "pushl %%edx;"
        "movl $243, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        "movl %%eax,%0;"
        "popl %%edx;"
        "popl %%ecx;"
        "popl %%ebx;"
        "popl %%eax;"
        : "=m" (res)
        : "m"(pid)
    );
    // TODO: Handle post return 
    /*if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    } */
    return (int) res;
}


// Description: Try to receive a message from process "rank" 
// Return value: Success - Size of the message, Failure - -1
int receive_mpi_message(int rank, char *message, ssize_t message_size)
{
    msgObj msg = {getpid(),rank,message,message_size};
    __asm__ (
        "pushl %%eax;"
        "pushl %%ebx;"
        "pushl %%ecx;"
        "pushl %%edx;"
        "movl $245, %%eax;"
        "movl %1, %%ebx;"
        "int $0x80;"
        "movl %%eax,%0;"
        "popl %%edx;"
        "popl %%ecx;"
        "popl %%ebx;"
        "popl %%eax;"
        : "=m" (res)
        : "m"(msg)
    );
    // TODO: Handle post return 
    /*if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    } */
    return (int) res;
}

#endif