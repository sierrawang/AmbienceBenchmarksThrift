#!/bin/bash

cd $HOME/AmbienceBenchmarksThrift/load_balancer/

python3 load_balancer_client2.py > $HOME/AmbienceBenchmarksThrift/benchmark/fedora_results/fedora_BEPS_client_vm_output.txt

cd $HOME/AmbienceBenchmarksThrift/benchmark/fedora_scripts/

python3 parse_BEPS_output.py ../fedora_results/fedora_BEPS_client_vm_output.txt > ../fedora_results/fedora_BEPS_client_vm_parsed.txt
