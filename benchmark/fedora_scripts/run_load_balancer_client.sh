#!/bin/bash

python3 $HOME/load_balancer/load_balancer_client2.py > ../fedora_results/fedora_BEPS_client_output.txt

python3 parse_BEPS_client_output.py ../fedora_results/fedora_BEPS_client_output.txt > ../fedora_results/fedora_BEPS_client_parsed.txt