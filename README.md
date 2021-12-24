# Parallel_Distributed_Systems_2
 
 Compile:
 
 $HOME/opt/usr/local/bin/mpicc -g -Wall main.c ../include/helpers/PowerOfTwo.c ../include/helpers/quickSelect.c ../include/functions/distributeByMedian.c ../include/helpers/removeElement.c ../include/functions/selfValidation.c ../include/functions/dataImportPivot.c -o main
 
 Run:
 
 $HOME/opt/usr/local/bin/mpirun -np 16 ./main
 
 Compiled and executed using Macintosh System
