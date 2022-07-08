#!/bin/bash

cd new_load_balancer

./read_endpoint &
sleep 1

./tweet_db &
sleep 1

./user_db server_con &
sleep 1

for (( i = 0; i < 5; i++ ))
do 
	./worker $i server_con server_con server_con &
done

sleep 1

./load_balancer server_con > ../benchmark/new_results/$1_lb_output.txt