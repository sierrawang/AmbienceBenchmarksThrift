#!/bin/bash

cd new_load_balancer

./write_endpoint server_con > ../benchmark/new_results/$1_we_output.txt
