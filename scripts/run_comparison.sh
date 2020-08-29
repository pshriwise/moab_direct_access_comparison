#!/bin/bash

MAX_THREADS=45

# run direct access version
for i in $( seq 1 $MAX_THREADS); do rate=$(OMP_NUM_THREADS=$i ./direct_access | grep second | cut -d" " -f 4); echo "$i $rate" >> da_data.txt; done

# run interface version
for i in $( seq 1 $MAX_THREADS); do rate=$(OMP_NUM_THREADS=$i ./interface | grep second | cut -d" " -f 4); echo "$i $rate" >> i_data.txt; done
