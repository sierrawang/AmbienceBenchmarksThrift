#!/bin/bash

docker rm -f $(docker ps -a)

# Compile everything in a container then kill it
docker run --net mynet --name make_load_balancer_con -v $HOME/AmbienceBenchmarksThrift:/root/app -it thrift_img /bin/bash benchmark/new_scripts/make_lb_logging.sh
docker rm -f make_load_balancer_con

docker run --net mynet --name server_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_server_side_services.sh fedora_test2 &

sleep 3

docker run -p 9121:9121 --net mynet --name edpt_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_wedpt.sh fedora_test2 server_con
