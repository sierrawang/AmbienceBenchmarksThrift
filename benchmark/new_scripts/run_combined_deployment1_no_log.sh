#!/bin/bash

docker rm -f $(docker ps -aq)

docker run -p 9121:9121 --net mynet --name one_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_all_services_no_log.sh $1
