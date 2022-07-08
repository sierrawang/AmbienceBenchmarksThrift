#!/bin/bash

cd new_load_balancer

make clean

make -f Makefile.log

./read_endpoint &
sleep 1

./tweet_db &
sleep 1

./user_db &
sleep 1

for (( i = 0; i < 5; i++ ))
do 
	./worker &
done

sleep 1

./load_balancer > ../benchmark/new_results/$1_lb_output.txt &

sleep 1

./write_endpoint > ../benchmark/new_results/$1_we_output.txt &