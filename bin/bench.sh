#!/bin/bash

binary=bin/nbody
# threads="1 2 4 8 16"
# sizes="18 20 22"
threads="1 2 4"
sizes="8 10 12"

for t in $threads; do
  for s in $sizes; do
    export OMP_NUM_THREADS=t
    echo $binary -n $(( 2**s )) 3> $RESULTS_DIR/`printf "%02d_%08d" $t $(( 2**s ))`.txt
  done
done
