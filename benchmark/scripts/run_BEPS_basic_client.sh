#!/bin/bash

cd /home/ubuntu/AmbienceBenchmarksThrift/load_balancer

# Run the client
python3 load_balancer_client.py > /home/ubuntu/AmbienceBenchmarksThrift/benchmark/results/BEPS_basic_linux_output.txt

# Parse results

