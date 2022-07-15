#!/bin/bash

cd new_load_balancer

./load_balancer $2 > ../benchmark/new_results/$1_lb_output.txt &

sleep 1

./write_endpoint lb > ../benchmark/new_results/$1_we_output.txt
