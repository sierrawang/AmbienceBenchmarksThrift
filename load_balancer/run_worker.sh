#!/bin/bash

cd load_balancer

for (( i = 0; i < 5; i++ ))
do 
	./worker $i > course_grain_results/linux_worker_$i.txt &
done

sleep 1

./load_balancer > ../benchmark/results/BEPS_lb_logging_linux_server_output.txt
