#!/bin/bash

cd /home/ubuntu/AmbienceBenchmarksThrift/load_balancer

# Run the client
python3 load_balancer_client.py > /home/ubuntu/AmbienceBenchmarksThrift/benchmark/results/BEPS_lb_logging_linux_output.txt

# Parse results
python3 parse_zero_logging.py /home/ubuntu/AmbienceBenchmarksThrift/benchmark/results/BEPS_lb_logging_linux_output.txt > /home/ubuntu/AmbienceBenchmarksThrift/benchmark/results/BEPS_lb_logging_linux_parsed.txt
