#!/bin/bash

docker rm -f $(docker ps -aq)

# Compile everything in a container then kill it
docker run --net mynet --name make_load_balancer_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/new_scripts/make_lb.sh
docker rm -f make_load_balancer_con

# Start the containers one by one
docker run --net mynet --name read_edpt -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_read_edpt.sh &
sleep 2

docker run --net mynet --name db_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_db.sh &
sleep 2

docker run -p 9089:9089 --net mynet --name worker_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_worker.sh $1 &

