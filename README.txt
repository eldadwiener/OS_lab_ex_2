To compile the python extension put your 'mpi_messages.h' header file in this
folder and type the following command in the terminal:

python setup.py build_ext -b .
or
python setup.py install

(The first will compile the module and place it in the current folder. The
second will compile and install the module in the python distribution, requires
root privileges)

If the compilation succeeds a new python extension is creatd. It can be
imported using:

>>> import MPI

This extension presents three functions that call your new system calls:
1) send_mpi_message
2) register_mpi
3) receive_mpi_message

You can use this functions in a python script or directly from the python
interpreter, type 'python' in the terminal and then the following commands:

>>> import MPI
>>> MPI.register_mpi()
>>> MPI.send_message(rank=0, message='hello')
>>> MPI.receive_mpi_message(rank=0, message_size=100)
'hello', 5

The syntax of the command can be found by typing the following in the python
interpreter:

>>> import MPI
>>> print MPI.register_mpi.__doc__