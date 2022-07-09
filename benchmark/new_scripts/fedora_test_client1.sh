#!/bin/bash

cd ../../new_load_balancer

make clean

make -f Makefile.log

./write_endpoint 10.0.1.10 > ../benchmark/new_results/$1_we_output.txt &

sleep 2

python3 test_write_endpoint.py localhost

killall write_endpoint

cd ../benchmark/new_scripts/

python3 parse_fedora_test1.py ../new_results/fedora_test5_we_output.txt > ../new_results/fedora_test5_we_parsed.txt
