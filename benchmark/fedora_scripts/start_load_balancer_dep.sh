#!/bin/bash

docker rm -f make_load_balancer_con
docker rm -f read_edpt
docker rm -f db_con
docker rm -f worker_con

# Compile everything in a container then kill it
docker run --net mynet --name make_load_balancer_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/fedora_scripts/make_load_balancer.sh
docker rm -f make_load_balancer_con

# Start the containers one by one
docker run --net mynet --name read_edpt -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_read_edpt.sh &
sleep 15

docker run --net mynet --name db_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_db.sh &
sleep 5

docker run -p 9089:9089 --net mynet --name worker_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_worker.sh &
sleep 5

echo DONE!