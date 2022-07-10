#!/bin/bash

cd new_load_balancer

./tweet_db &
sleep 1
./user_db db_con

