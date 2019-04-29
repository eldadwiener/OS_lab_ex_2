#!/usr/bin/python

import MPI
import os

def test():
    """Ex2 test"""

    MPI.register_mpi()
    
    #
    # Fork the parent
    #
    cpid = os.fork()
    if (cpid == 0):
        #
        # In child, send a message to the parent who should have a rank of 0
        #
        MPI.send_mpi_message(0, 'hello')
        
        #
        # Terminate the child process
        #
        os._exit(0)

    #
    # Wait for the child to terminate
    #
    os.wait()
    
    #
    # read the message sent from the child, that had a rank of 1
    #
    print MPI.receive_mpi_message(1, 100)
    print 'Test Finished'

    
if __name__ == '__main__':
    test()
