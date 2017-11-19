/* Installation ----- https://tecadmin.net/install-go-on-ubuntu/# */
package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"reflect"
	"bytes"
	"unicode/utf8"
	"bufio"
	"os"
	"strings"
)

type keyValueRequestDataArray struct {
	KeyValuePair []keyValueRequestDataFormat `json:"keyvalue"`
}

type keyValueRequestDataFormat struct {
	Key string `json:"key"`
	Value string `json:"value,omitempty"`
}

var server = make(map[int]string)
var ports = make(map[int]string)
var server_cnt = 0

/* TODO: improve hashing -> Consistent hashing
 * hashing implemented using simple modulo function
 */
func hashing (KeyValuePair []keyValueRequestDataFormat)([][]keyValueRequestDataFormat) {
	server_data_cnt := make([]int, server_cnt)
	server_data := make([][]keyValueRequestDataFormat, server_cnt)

	x := 0
	var row []keyValueRequestDataFormat
	for _,v := range KeyValuePair {
		temp := v.Key
		i := 0;
		/* Find ascii value of string for hashing */
		for len(temp) > 0 {
			r, size := utf8.DecodeRuneInString(temp)
			i = i + int(r)
			temp = temp[size:]
		}
		hash := i % server_cnt
		row = server_data[hash]
		row = append(row, keyValueRequestDataFormat{v.Key, v.Value})
		server_data[hash] = row
		server_data_cnt[hash]++;
		x++;
	}
	for index,val := range server_data {
		fmt.Println(index, val);
	}
	return server_data
}

func formatDataForRequest(indx int, data []keyValueRequestDataFormat) ([]byte) {
	var slice []keyValueRequestDataFormat
	var i int
	var v keyValueRequestDataFormat
	var post_data keyValueRequestDataArray
	var post_data_JSON []byte
	var err_data error
	var KeyValuePair_cnt int

	fmt.Println("SERVER :::: "+server[indx])
	KeyValuePair_cnt = len(data)
	fmt.Println("Total number of keyVals = ", KeyValuePair_cnt)
	if KeyValuePair_cnt == 0 {
		return post_data_JSON
	}
	slice = make([]keyValueRequestDataFormat, KeyValuePair_cnt)
	for i,v = range data {
		fmt.Println("\n", v.Key, v.Value)
		slice[i] = v
	}

	post_data = keyValueRequestDataArray{slice}
	post_data_JSON, err_data = json.Marshal(post_data)
	if err_data != nil {
		fmt.Println("Error in JSON formatting !! ")
	}
	return post_data_JSON
}

func makeRequest(indx int, post_data_JSON []byte, request_type string ) (string) {
	var err_data error
	var req *http.Request
	var client *http.Client
	var resp *http.Response

	url := server[indx]

	if request_type == "PUT" || request_type == "POST" {
		req, err_data = http.NewRequest(request_type,
										url,
										bytes.NewBuffer(post_data_JSON))
		client = &http.Client{}
		resp, err_data = client.Do(req)
		if err_data != nil {
			panic(err_data)
		}
		defer resp.Body.Close()
		body, _ := ioutil.ReadAll(resp.Body)
		return string(body)
	} else {
		fmt.Println("GET REQ STARTING !!\n")
		resp, err_data = http.Get(url)

		if err_data != nil {
			panic(err_data)
		}
		defer resp.Body.Close()
		body, _ := ioutil.ReadAll(resp.Body)
		return string(body)
	}
}

/* GetHandler handles the index route */
func GetHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("GET HANDLER STARTING !!!!\n")
	var slice []keyValueRequestDataFormat

	for indx, _ := range server {
		post_data_JSON :=  []byte("")
		body := makeRequest(indx, post_data_JSON, "GET")

		var msg keyValueRequestDataArray
		err := json.Unmarshal([]byte(body), &msg)

		if err != nil {
			panic(err)
		}

		for _,v := range msg.KeyValuePair {
			slice = append(slice, v)
		}
	}
	//restore_server_details()

	response_data := keyValueRequestDataArray{slice}
	//fmt.Println(response_data)
	fmt.Fprint(w, response_data)
	fmt.Fprint(w, "\nGet Done !!\n")
}

func PostHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == "POST" {
		decodeJson := json.NewDecoder(r.Body)
		var msg keyValueRequestDataArray
		err := decodeJson.Decode(&msg)
		if err != nil {
			fmt.Println("Error here 1\n");
			panic(err)
		}

		/* find the server using hash */
		server_data := hashing(msg.KeyValuePair)

		var slice []keyValueRequestDataFormat
		for index, data := range server_data {
			if len(data) == 0 {
				fmt.Println("url %s has no data", server[index])
				continue
			}

			// formatting data for request
			post_data_JSON := formatDataForRequest(index, data)
			fmt.Println(post_data_JSON)

			// forward to server
			body := makeRequest(index, post_data_JSON, "POST")
			//fmt.Println(body)

			var msg keyValueRequestDataArray
			err := json.Unmarshal([]byte(body), &msg)
			if err != nil {
				fmt.Println("Error here 2\n");
				panic(err)
			}

			for _,v := range msg.KeyValuePair {
				slice = append(slice, v)
			}

		}
		response_data := keyValueRequestDataArray{slice}
		//fmt.Println(response_data)
		fmt.Fprint(w, response_data)
	} else {
		fmt.Println("Protocol NOT supported !!")
	}
	//restore_server_details()
}

func PutHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == "PUT" {
		decodeJson := json.NewDecoder(r.Body)

		var msg keyValueRequestDataArray
		err := decodeJson.Decode(&msg)

		if err != nil {
			panic(err)
		}
		fmt.Println(reflect.TypeOf(msg.KeyValuePair))
		fmt.Println(msg)

		/* find the server using hash */
		server_data := hashing(msg.KeyValuePair)

		for indx, data := range server_data {
			if len(data) == 0 {
				continue
			}
			// formatting data for request
			post_data_JSON := formatDataForRequest(indx, data)
			fmt.Println(post_data_JSON)

			// forward to server
			body := makeRequest(indx, post_data_JSON, "PUT")
			fmt.Println(body)
			// fmt.Println("response Status:", resp.Status)
			// fmt.Println("response Headers:", resp.Header)
			// body, _ := ioutil.ReadAll(resp.Body)
			// fmt.Println("response Body:", string(body))
		}
		//restore_server_details()
	}
}

func fetch(w http.ResponseWriter, r *http.Request) {
	make_server_addresses("/fetch")

	if r.Method == "POST" {
		PostHandler(w, r)
	} else if r.Method == "GET" {
		GetHandler(w, r)
	} else {
		fmt.Print("Method not Supported on this end point!!")
		fmt.Fprint(w, "Method not Supported on this end point!!")
	}
}

func query(w http.ResponseWriter, r *http.Request) {
	make_server_addresses("/query")

	if r.Method == "POST" {
		PostHandler(w, r)
	} else if r.Method == "GET" {
		GetHandler(w, r)
	} else {
		fmt.Print("Method not Supported on this end point!!")
		fmt.Fprint(w, "Method not Supported on this end point!!")
	}
}

func set(w http.ResponseWriter, r *http.Request) {
	make_server_addresses("/set")

	if r.Method == "PUT" {
		PutHandler(w, r)
	} else {
		fmt.Print("Method not Supported on this end point!!")
		fmt.Fprint(w, "Method not Supported on this end point!!")
	}
}

func make_server_addresses(endpoint string){
	for i, s := range server {
		server[i] = s+":"+ports[i]+endpoint
	}
}

func restore_server_details() {
	file, err := os.Open(os.Args[2])
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	i := 0
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		server_url := "http://"
		result := strings.Split(scanner.Text(), " ")
		server_url += result[0]
		server_port := result[1]
		server[i] = server_url
		ports[i] = server_port
		fmt.Println(server_url, server_port)
		i++
	}
	server_cnt = i + 1

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}
}

func init() {
	log.SetFlags(log.Lmicroseconds | log.Lshortfile)
	flag.Parse()
	restore_server_details()
	server_cnt = len(server)
}

func main() {
	/* flagPort is the open port the application listens on */
	var (flagPort = flag.String("port", os.Args[1], "Port to listen on"))
	mux := http.NewServeMux()
	mux.HandleFunc("/query", query)
	mux.HandleFunc("/fetch", fetch)
	mux.HandleFunc("/set", set)

	log.Printf("listening on port %s", *flagPort)
	log.Fatal(http.ListenAndServe(":"+*flagPort, mux))
}
