#!/bin/bash

THREADS=(1 2 4 8 16 32 64)
CLUSTERS=(10 40 80)
FILES=(color edge texture)

make

for thread in ${THREADS[@]}; do
  export OMP_NUM_THREADS=$thread
  #create directories for threads
  if [ ! -d "benchmarks/threads_$thread" ] 
  then
    mkdir "benchmarks/threads_$thread"
  fi
  
  for cluster in ${CLUSTERS[@]}; do
    #create sub-directories for clusters
    if [ ! -d "benchmarks/threads_$thread/clusters_$cluster" ] 
    then
      mkdir "benchmarks/threads_$thread/clusters_$cluster"
    fi

    for file in ${FILES[@]}; do
    #create sub-sub-directories for each file and computation io
    if [ ! -d "benchmarks/threads_$thread/clusters_$cluster/$file" ] 
    then
      mkdir "benchmarks/threads_$thread/clusters_$cluster/$file"
    fi

    #touch "benchmarks/threads_$thread/clusters_$cluster/$file/computation.txt" "benchmarks/threads_$thread/clusters_$cluster/$file/io.txt"

      for (( i=1; i<=12; i++ )) do  
        ./seq_main -i Image_data/"$file"17695.bin -n $cluster -o -b >> "benchmarks/threads_$thread/clusters_$cluster/$file/computation.txt" 2>> "benchmarks/threads_$thread/clusters_$cluster/$file/io.txt"
      done
    done
  done
done



