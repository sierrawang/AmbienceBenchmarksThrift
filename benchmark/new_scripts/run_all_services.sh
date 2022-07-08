#!/bin/bash

cd new_load_balancer

make clean

make -f Makefile.log

./read_endpoint &
sleep 1

./tweet_db &
sleep 1

./user_db one_con &
sleep 1

for (( i = 0; i < 5; i++ ))
do 
	./worker $i one_con one_con one_con &
done

sleep 1

./load_balancer one_con > ../benchmark/new_results/$1_lb_output.txt &

sleep 1

./write_endpoint one_con > ../benchmark/new_results/$1_we_output.txt
