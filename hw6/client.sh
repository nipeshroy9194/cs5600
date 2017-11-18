#!/bin/bash
# A simple client doing curl on distributed key-value store

router_port=8080
json_filename=application.json

curl -iH 'Content-Type:application/json' -X PUT -d @$json_filename localhost:$router_port/set
curl -iH 'Content-Type: application/json' -d @$json_filename -X POST localhost:$router_port/query
curl -iH 'Content-Type: application/json' -d @$json_filename -X POST localhost:$router_port/fetch
curl -iH "Accept: application/json" "localhost:$router_port/fetch"
curl -iH "Accept: application/json" "localhost:$router_port/query"
