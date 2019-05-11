#ifndef _MY_MPI_H_
#define _MY_MPI_H_

#include <linux/list.h>

typedef struct global_mpi_struct g_mpi_t;
typedef struct msg_queue_struct msg_q_t;

extern list_t g_mpi_head;
extern int nextRank;

struct global_mpi_struct{
    list_t mylist;
    int rank;
    list_t taskMsgHead;
}

struct msg_queue_struct {
   list_t mylist;
   char* msg;
   int msgsize;
   int senderRank; 
}



#endif //_MY_MPY_H_