#!/bin/bash

cd new_load_balancer

for (( i = 0; i < 5; i++ ))
do 
	./worker $i db_con db_con read_edpt &
done

sleep 1

./load_balancer worker_con > ../benchmark/new_results/$1_lb_output.txt &

sleep 1

./write_endpoint worker_con > ../benchmark/new_results/$1_we_output.txt
