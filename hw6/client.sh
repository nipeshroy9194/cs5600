#!/bin/bash
# A simple client doing curl on distributed key-value store

router_port=8080
json_put=put.json
json_post=post.json

curl -iH 'Content-Type: application/json' -X PUT -d @$json_put localhost:$router_port/set
curl -iH 'Content-Type: application/json' -X POST -d @$json_post localhost:$router_port/query
curl -iH 'Content-Type: application/json' -X POST -d @$json_post localhost:$router_port/fetch
curl -iH "Accept: application/json" -X GET localhost:$router_port/fetch
curl -iH "Accept: application/json" -X GET localhost:$router_port/query
