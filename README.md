MPI Notes:

MPI applications - compilation:
    mpicc test.c -o test
    mpicxx test.cpp -o test

execution:

Launch 16 processes on the local node
    mpiexec -np 16 ./test

Launch 16 processes on 4 notes (each has 4 cores)
    mpiexec -hosts h1;4, h2:4, h3:4, h4:4 -np 16 ./test

    mpiexec -hosts h1,h2,h3,h4 -np 16 ./test
    // so h1 will have the 1st, 5th, 9th and 14th processes

If there are many notes, create a host file:
    cat hf
        h1:4
        h2:2

    mpiexec - hostfile hf -np 16 ./test

for our project!

    g++ -c synapse.cpp
    g++ -c main.cpp
    mpicxx synapse.o main.o -o main
    mpiexec ./main

to execute:
    mpiexec -np 6 --hosts node,node1,node2,node3,node4,node5 ./main
    mpiexec -np 6 -hostsfile nodehosts ./main

