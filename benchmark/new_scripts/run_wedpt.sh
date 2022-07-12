#!/bin/bash

cd new_load_balancer

./write_endpoint $2 > ../benchmark/new_results/$1_we_output.txt
