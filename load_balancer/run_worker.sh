#!/bin/bash

cd load_balancer

make

for (( i = 0; i < 64; i++ ))
do 
	./worker $i > course_grain_results/linux_worker_$i.txt &
done

sleep 1

./load_balancer > course_grain_results/linux_load_balancer.txt
