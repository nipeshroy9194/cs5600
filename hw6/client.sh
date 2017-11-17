#!/bin/bash
# A simple client doing curl

router=http://localhost:8080
json_filename=application.json

curl -vX PUT $router -d @$json_filename -H "Content-Type:application/json"
