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

python3 load_balancer_client.py > course_grain_results/linux_course_grain_parsed.txt

echo DONE!
