from shutil import copyfile

files_to_copy = [\
    'kernel/mpi_messages.c',\
    'include/linux/mpi_messages.h',\
    'include/linux/sched.h'\
    'arch/i386/kernel/entry.S'\
    'mpi_messages_api.h']

for file in files_to_copy:
    copyfile('/mnt/hgfs/shared_folder/OS_lab_ex_2/' + file, '/usr/src/linux-2.4.18-14custom/'+ file)