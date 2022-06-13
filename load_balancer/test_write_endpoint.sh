#!/bin/bash

docker rm -f worker_con
docker rm -f db_con
docker rm -f read_edpt

docker run -p 3003:3000 --net mynet --name read_edpt -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_read_edpt.sh &

sleep 5

docker run -p 3000:3000 --net mynet --name db_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_db.sh &

sleep 5

docker run -p 3001:3000 --net mynet --name worker_con -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_worker.sh &

sleep 5

docker run -p 3002:3000 --net mynet --name write_edpt -v /home/ubuntu/AmbienceBenchmarksThrift:/root/app thrift_img /bin/bash load_balancer/run_write_edpt.sh &

sleep 5

python3 test_write_endpoint.py

python3 parse_write_endpoint.py > ambience_write_endpoint_parsed.txt

echo DONE!