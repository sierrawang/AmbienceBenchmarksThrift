#!/bin/bash

# This script is for running the load balancer client inside a container 
cd load_balancer/

python3 load_balancer_client2.py > ../benchmark/fedora_results/fedora_BEPS_client_client_in_container_output.txt