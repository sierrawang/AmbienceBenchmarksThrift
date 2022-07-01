#!/bin/bash

cd $HOME/AmbienceBenchmarksThrift/load_balancer/

python3 test_capacity.py > $HOME/AmbienceBenchmarksThrift/benchmark/fedora_results/fedora_BEPS_client_capacity_output.txt

cd $HOME/AmbienceBenchmarksThrift/benchmark/fedora_scripts/

python3 parse_test_capacity_output.py ../fedora_results/fedora_BEPS_client_capacity_output.txt > ../fedora_results/fedora_BEPS_client_capacity_parsed.txt