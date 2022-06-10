#!/bin/bash

make

for (( i = 0; i < 1; i++ ))
do 
	./worker i > course_grain_results/linux_worker_$i.txt &
done

./load_balancer > course_grain_results/linux_load_balancer.txt
