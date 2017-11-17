package main

import (
	"encoding/json"
	"encoding/base64"
	"net/http"
	"log"
	"fmt"
	"strings"
	"io"
	"io/ioutil"
	"unicode/utf8"
)

func printBody(req *http.Request) {
	body, err := ioutil.ReadAll(req.Body)
	if err != nil {
		panic(err)
	}
	log.Println(req.Body)
	log.Println(string(body))
}

type MessageArr struct {
	Data []Message `json:"data"`
}

type Message struct {
	Key string `json:"key"`
	Value string `json:"value"`
}

type ServerLocWithKeyHash struct {
	Key string
	Value string
	surl string
}

func Cleaner(w http.ResponseWriter, r *http.Request) {
	server := make(map[int]string)
	server[0] = "http://localhost:8081"
	server[1] = "http://localhost:8082"
	server[2] = "http://localhost:8083"
	server[3] = "http://localhost:8084"
	server_cnt := len(server)
	server_data_cnt := make([]int, server_cnt)
	server_data := make([][]Message, server_cnt)
	/*itr := 0
	for itr < server_cnt {
		server_data[itr] = make([]Message)
		itr++
	}*/

	defer r.Body.Close()
	if r.Body == nil {
		http.Error(w, "Please send Request body", 400)
		return
	}

	var m MessageArr
	dec := json.NewDecoder(r.Body)
	err := dec.Decode(&m);
	if err != nil {
		panic(err)
	}
	fmt.Println(m.Data);

	key_cnt := 0
	for range m.Data {
		key_cnt++;
	}

	x := 0
	var row []Message
	for _,v := range m.Data {
		temp := v.Key
		i := 0;
		for len(temp) > 0 {
			r, size := utf8.DecodeRuneInString(temp)
			i = i + int(r)
			temp = temp[size:]
		}
		fmt.Println(i)
		hash := i % 4
		fmt.Println(v.Key, v.Value, hash)
		row = server_data[hash]
		row = append(row, Message{v.Key, v.Value})
		server_data[hash] = row
		server_data_cnt[hash]++;
		x++;
	}
	for index,val := range server_data {
		fmt.Println(index, val);
	}
	fmt.Println(server_data);
}

func handler(w http.ResponseWriter, r *http.Request) {
	database := make(map[string][]byte)
	database["key1"] = []byte("A")
	database["key2"] = []byte("B")
	database["key3"] = []byte("C")
	database["key4"] = []byte("D")

	switch r.Method {
	case "GET":
		w.Header().Set("Content-Type", "application/json; charset=utf-8")
		w.Header().Set("Content-Transfer-Encoding", "BASE64")

		mapD := make(map[string][]byte)
		for key, value := range database {
			fmt.Println("Key:", key, "Value:", value)

			/* Serve the resource. */
			length := base64.RawStdEncoding.EncodedLen(len(value))
			base64val := make([]byte, length)
			base64.RawStdEncoding.Encode(base64val, value)
			log.Printf("base64: %s\n", base64val)

			mapD[key] = base64val
		}
		mapB, _ := json.Marshal(mapD)
		w.Write(mapB)
	case "POST":
		// Create a new record.
	case "PUT":
		// Update an existing record.
		Cleaner(w, r);
	case "DELETE":
		// Remove the record.
	default:
		// Give an error message.
	}
	return
}

func main() {
	/* register you http handler */
	http.HandleFunc("/", handler)
	/* Listening on port 8080 on the router */
	fmt.Println("SERVER LISTENING");
	err := http.ListenAndServe(":8080", nil)
	log.Fatal(err)
}

func temp() {
	const jsonStream = `
	{"Name": "Ed", "Text": "Knock knock."}
	{"Name": "Sam", "Text": "Who's there?"}
	{"Name": "Ed", "Text": "Go fmt."}
	{"Name": "Sam", "Text": "Go fmt who?"}
	{"Name": "Ed", "Text": "Go fmt yourself!"}
	`
	type Message struct {
		Name, Text string
	}
	dec := json.NewDecoder(strings.NewReader(jsonStream))
	for {
		var m Message
		if err := dec.Decode(&m); err == io.EOF {
			break
		} else if err != nil {
			log.Fatal(err)
		}
		fmt.Printf("%s: %s\n", m.Name, m.Text)
	}
}
