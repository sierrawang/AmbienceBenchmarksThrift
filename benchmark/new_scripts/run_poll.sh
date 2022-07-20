#!/bin/bash

cd poll

./poll > ../benchmark/new_results/poll_$1_output.txt &
./poll_bench_agent > ../benchmark/new_results/poll_bench_agent_$1_output.txt &
./poll_param_bench_agent > ../benchmark/new_results/poll_param_bench_agent_$1_output.txt
