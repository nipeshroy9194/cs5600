#!/bin/bash
# A simple client doing curl on distributed key-value store

router_port=$2
router_ip=$1
json_put=put.json
json_post=post.json

curl -iH 'Content-Type: application/json' -X PUT -d @$json_put $router_ip:$router_port/set
curl -iH 'Content-Type: application/json' -X POST -d @$json_post $router_ip:$router_port/query
curl -iH 'Content-Type: application/json' -X POST -d @$json_post $router_ip:$router_port/fetch
curl -iH "Accept: application/json" -X GET $router_ip:$router_port/fetch
curl -iH "Accept: application/json" -X GET $router_ip:$router_port/query
