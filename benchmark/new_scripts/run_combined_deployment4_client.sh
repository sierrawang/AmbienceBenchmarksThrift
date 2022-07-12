#!/bin/bash

docker rm -f $(docker ps -aq)

# Compile everything in a container then kill it
docker run --net mynet --name make_load_balancer_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/new_scripts/make_lb.sh
docker rm -f make_load_balancer_con

docker run -p 9121:9121 --net mynet --name one_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_wedpt.sh $1 10.0.1.10
