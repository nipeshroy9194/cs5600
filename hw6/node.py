#!/usr/bin/env python
"""
Very simple HTTP server in python.
Usage::
    ./dummy-web-server.py [<ip_address>] [<port>]
Send a GET request::
    curl http://localhost
Send a HEAD request::
    curl -I http://localhost
Send a POST request::
    curl -d "foo=bar&bin=baz" http://localhost
"""
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import SocketServer
from urlparse import parse_qs
import json
import string

key_value = {'abc':'123', 'xyz':'098'}
KEY_NOT_FOUND = "KEY NOT FOUND"

'''
sudo lsof -i -P -n | grep LISTEN
'''
class S(BaseHTTPRequestHandler):

    def _set_headers(self, header_code):
        self.send_response(header_code)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    '''
    curl localhost http://localhost:5000/?key=tret,abc
    '''
    def do_GET(self):
        print "----- GET IN SERVER !! ------"
        response_data = []
        if not (self.path == '/fetch' or self.path == '/query'):
            self._set_headers(403)
            self.wfile.write(response = {"keyvalue": response_data})
            return
        self._set_headers(200)
        for key in key_value.keys():
            if self.path == '/fetch':
                row = {"key": key, "value": key_value[key]}
            elif self.path == '/query':
                row = {"key": key, "value": "True"}
            response_data.append(row)
        response = {"keyvalue": response_data}
        resp_json = json.dumps(response)
        self.wfile.write(resp_json) 

    '''
    curl --request PUT 'localhost:5000' --data '[{"mnop":"345"}, {"tret":"555"}]'
    '''
    def do_PUT(self):
        print "----- PUT IN SERVER !! ------"
        response_data = []
        if not self.path == '/set':
            self._set_headers(403)
            self.wfile.write(response = {"keyvalue": response_data})
            return
        self._set_headers(200)
        print self.headers
        length = int(self.headers['Content-Length'])
        content = self.rfile.read(length)
        content_lst = json.loads(content)
        print content_lst
        for k_v in content_lst['keyvalue']:
            key = k_v['key']
            val = k_v['value']
            print key, '------',val
            key_value[key] = val 

    def do_HEAD(self):
        self._set_headers()
        
    def do_POST(self):
        print "----- POST IN SERVER  !! ------"
        response_data = []
        if not (self.path == '/fetch' or self.path == '/query'):
            self._set_headers(403)
            self.wfile.write(response = {"keyvalue": response_data})
            return
        self._set_headers(200)
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        content = self.rfile.read(content_length) # <--- Gets the data itself
        content_lst = json.loads(content)
        print content_lst
        for k_v in content_lst['keyvalue']:
            key = k_v['key']
            if key in key_value:
                if self.path == '/fetch':
                    row = {"key": key, "value": key_value[key]}
                elif self.path == '/query':
                    row = {"key": key, "value": "True"}
            else:
                if self.path == '/fetch':
                    row = {"key": key, "value": KEY_NOT_FOUND}
                elif self.path == '/query':
                    row = {"key": key, "value": "False"}
            response_data.append(row)
        response = {"keyvalue": response_data}
        resp_json = json.dumps(response)
        print "Response :: ", resp_json
        self.wfile.write(resp_json) 

def run(server_class=HTTPServer, handler_class=S, host='', port=80):
    server_address = (host, port)
    httpd = server_class(server_address, handler_class)
    print 'Starting httpd...'
    httpd.serve_forever()

if __name__ == "__main__":
    from sys import argv

    if len(argv) == 3:
        run(host=argv[1].encode('string-escape'), port=int(argv[2]))
    else:
        run()
