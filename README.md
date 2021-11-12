# PC_Project_2
Parallelization of Dijkstra’s Single Source Shortest Path (SSSP) Algorithm

Compiling main.cpp use:
    g++ main.cpp -o main
Run with:
    ./main

Compiling pMainMpi.cpp use:
    mpic++ pMainMpi.cpp -o pMainMpi
Run with:
    mpirun -np insert_num_procs ./pMainMpi

Compiling pMainOmp.cpp use:
    g++ -fopenmp pMainOmp.cpp -o pMainOmp
Run with:
    ./pMainOmp
Set number of threads with the following in console:
    export OMP_NUM_THREADS=insert_num_threads

Edit the function genRandomGraph in main to change the number of vertices
