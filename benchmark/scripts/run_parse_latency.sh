#!/bin/bash

python3 parse_poll_output.py ../results/poll_bench_agent_output.txt > ../results/poll_bench_agent_parsed.txt
python3 parse_poll_param_output.py ../results/poll_param_bench_agent_output.txt > ../results/poll_param_bench_agent_parsed.txt