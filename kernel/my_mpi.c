#include <linux/my_mpi.h>
#include <asm-i386/errno.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

list_t g_mpi_head = LIST_HEAD_INIT(g_mpi_head);
int nextRank = 0;

int sys_register_mpi(void)
{
    if(current->rank != -1) // already registered
        return current->rank;
    g_mpi_t* newNode = (g_mpi_t*)kmalloc(sizeof(g_mpi_t),GFP_KERNEL); // TODO: check if allocated correctly
    if(newNode == NULL)
        return -ENOMEM;
    // update the node and the task struct to register the process.
    current->rank = nextRank++;
    newNode->rank = current->rank;
    newNode->taskMsgHead = current->taskMsgHead;
    list_add_tail( &(newNode->mylist), &g_mpi_head );
    return current->rank;
}

int sys_send_mpi_message(int rank, const char* message, ssize_t message_size)
{
    // check parameters
    if ( message == NULL || message_size < 1)
        return -EINVAL;
    // make sure current process is registered in the mpi
    if ( current->rank == -1)
        return -ESRCH;
    // search for rank in the linked list
    list_t* pos;
    BOOL found = FALSE;
    list_for_each(pos,&g_mpi_head)
    {
        if( list_entry(pos,g_mpi_t, mylist)->rank == rank)
        {
            found = TRUE;
            break;
        }
    }
    if (found == FALSE)
        return -ESRCH; // rank was not found in the mpi processes list
    // both sender and receiver are in the mpi list, copy the message from the user
    char* copiedMsg = (char*)kmalloc(message_size*sizeof(char), GFP_KERNEL);
    if ( copiedMsg == NULL )
        return -ENOMEM;
    if ( copy_from_user(copiedMsg, message, message_size) )
    {
        kfree(copiedMsg);
        return -EFAULT;
    }
    // done copying, create new node for the message list
    msg_q_t* msgNode = (msg_q_t*)kmalloc(sizeof(msg_q_t), GFP_KERNEL);
    if ( msgNode == NULL )
    {
        kfree(copiedMsg);
        return -ENOMEM;
    }
    // update node data, and add to the message list
    msgNode->msg = copiedMsg;
    msgNode->msgsize = message_size;
    msgNode->senderRank = current->rank;
    list_add_tail( &(msgNode->mylist), &(list_entry(pos,g_mpi_t, mylist)->taskMsgHead) );
    return 0;
}

int sys_receive_mpi_message(int rank, char* message, ssize_t message_size)
{
    // check parameters
    if ( message == NULL || message_size < 1)
        return -EINVAL;
    // make sure current process is registered in the mpi
    if ( current->rank == -1)
        return -ESRCH;
    // search for rank in the linked list
    list_t *pos;
    BOOL found = FALSE;
    list_for_each(pos,&g_mpi_head)
    {
        if( list_entry(pos,g_mpi_t, mylist)->rank == rank)
        {
            found = TRUE;
            break;
        }
    }
    if (found == FALSE)
        return -ESRCH; // rank was not found in the mpi processes list
    // both sender and receiver are in the mpi list,
    // check if there is a message waiting from 'rank'
    found = FALSE;
    list_for_each(pos,&(current->taskMsgHead))
    {
        if( list_entry(pos,msg_q_t, mylist)->senderRank == rank)
        {
            found = TRUE;
            break;
        }
    }
    if ( found == FALSE ) // no message waiting from rank, even though rank is indeed in the mpi list
        return -EFAULT;
    
    msg_q_t* curMsg = list_entry(pos,msg_q_t, mylist);
    // found is TRUE, so pos is the relevant message entry from rank to the receiver
    // TODO : what if message_size if smaller than the actual message size?
    if ( copy_to_user( message, curMsg->msg , curMsg->msgsize) )
        return -EFAULT;
    ssize_t copiedSize = curMsg->msgsize; //TODO: is it possible that copy to user copied less?    
    // done copying the message, remove the entry
    list_del( &(curMsg->mylist) );
    kfree(curMsg->msg);
    kfree(curMsg);

    return copiedSize;
}

int copyMPI(struct task_struct* p)
{
    // init the msg list
    INIT_LIST_HEAD(&p->taskMsgHead);
    // register the child process as a new process in the mpi list
    g_mpi_t* newNode = (g_mpi_t*)kmalloc(sizeof(g_mpi_t),GFP_KERNEL); // TODO: check if allocated correctly
    if(newNode == NULL)
        return -ENOMEM; // TODO: what do we do here?
    // update the node and the task struct to register the process.
    p->rank = nextRank++;
    newNode->rank = p->rank;
    newNode->taskMsgHead = p->taskMsgHead;
    list_add_tail( &(newNode->mylist), &g_mpi_head );
    // copy all of the messages from the parent to the child process
    list_t *pos;
    BOOL failed = FALSE;
    list_for_each(pos,&current->taskMsgHead)
    {
        msg_q_t* curMsg = list_entry(pos,msg_q_t, mylist);
        msg_q_t* msgNode = (msg_q_t*)kmalloc(sizeof(msg_q_t), GFP_KERNEL);
        if ( msgNode == NULL)
        {
            failed = TRUE;
            break;
        }
        char* copiedMsg = (char*)kmalloc(curMsg->msgsize * sizeof(char), GFP_KERNEL);
        if ( copiedMsg == NULL)
        {
            failed = TRUE;
            break;
        }
        // copy the message from the parent
        int i;
        for ( i = 0; i <= curMsg->msgsize; ++i)
            copiedMsg[i] = curMsg->msg[i];
        msgNode->msg = copiedMsg;
        msgNode->msgsize = curMsg->msgsize;
        msgNode->senderRank = curMsg->senderRank;
        // finally add the new msg to the list
        list_add_tail( &msgNode->mylist, &p->taskMsgHead);
    }
    // free memory if failed to copy
    if ( failed == TRUE)
    {
        list_for_each(pos,&p->taskMsgHead)
        {
            msg_q_t* curMsg = list_entry(pos,msg_q_t, mylist);
            kfree(curMsg->msg);
            kfree(curMsg);
        }
        return -ENOMEM; // TODO: what to do in this situation?
    }
    // done copying all messages
    return 0;
}



void exit_MPI(void)
{
    if(current->rank == -1)
        return; // not registered for MPI
    // we are in MPI, remove us from the mpi process list
    list_t *pos;
    list_for_each(pos,&g_mpi_head)
    {
        if( list_entry(pos,g_mpi_t, mylist)->rank == current->rank)
            break;
    }   
    list_del( pos );
    kfree(list_entry(pos,g_mpi_t, mylist));
    if (g_mpi_head.prev == &g_mpi_head && g_mpi_head.next == &g_mpi_head) // list is empty, need to reset rank numbers
        nextRank = 0;
    // finally, delete all messages in this process's queue
    list_for_each(pos,&current->taskMsgHead)
    {
        msg_q_t* curMsg = list_entry(pos,msg_q_t, mylist);
        kfree(curMsg->msg);
        kfree(curMsg);
    }
}
