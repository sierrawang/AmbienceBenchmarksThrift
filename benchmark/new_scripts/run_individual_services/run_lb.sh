#!/bin/bash

cd new_load_balancer

./load_balancer $2 > ../benchmark/new_results/$1_lb_output.txt
