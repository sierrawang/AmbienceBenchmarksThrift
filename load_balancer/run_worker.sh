#!/bin/bash

cd load_balancer

make

for (( i = 0; i < 5; i++ ))
do 
	./worker $i > fine_grain_results/linux_worker_$i.txt &
done

sleep 1

./load_balancer > fine_grain_results/linux_load_balancer.txt
