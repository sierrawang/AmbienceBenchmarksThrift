#!/bin/bash

docker rm -f $(docker ps -a)

docker run -p 9121:9121 --net mynet --name edpt_con -v $HOME/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash benchmark/new_scripts/run_wedpt.sh fedora_test6
