#!/bin/bash

cd ../../load_balancer/

make clean

make

./write_endpoint > ../benchmark/fedora_results/fedora_BEPS_client_host_output.txt &

sleep 5

python3 test_write_endpoint3.py

cd ../benchmark/fedora_scripts/

python3 parse_BEPS_output.py ../fedora_results/fedora_BEPS_client_host_output.txt > ../fedora_results/fedora_BEPS_client_host_parsed.txt
