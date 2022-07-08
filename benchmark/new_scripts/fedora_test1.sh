#!/bin/bash

docker rm -f $(docker ps -a)

docker run -p 9121:9121 --net mynet --name one_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_all_services.sh fedora_test1
