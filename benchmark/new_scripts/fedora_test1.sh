#!/bin/bash

docker rm -f $(docker ps -a)

docker run --net mynet --name one_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_all_services.sh 