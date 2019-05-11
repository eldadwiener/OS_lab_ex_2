#!/bin/bash

PATH1="/usr/src/linux-2.4.18-14custom/"
echo $PWD  
cp  "$PWD"/include/linux/my_mpi.h "$PATH1"include/linux/
cp  "$PWD"/include/linux/sched.h "$PATH1"include/linux/
cp  "$PWD"/kernel/exit.c "$PATH1"kernel/
cp  "$PWD"/kernel/fork.c "$PATH1"kernel/
cp  "$PWD"/kernel/my_mpi.c "$PATH1"kernel/
