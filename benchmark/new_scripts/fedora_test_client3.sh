#!/bin/bash

cd ../../new_load_balancer

make clean

make

./write_endpoint 10.0.1.10 > ../benchmark/new_results/$1_we_output.txt

