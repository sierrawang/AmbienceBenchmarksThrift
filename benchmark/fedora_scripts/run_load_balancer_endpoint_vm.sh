#!/bin/bash

cd ../../load_balancer/

make clean

make

./write_endpoint > ../benchmark/fedora_results/fedora_BEPS_client_endpoint_vm_output.txt &
