build: client

client:
	g++ -std=c++11 -Wall -Wextra client.cpp requests.cpp helper.cpp -o client
run: client
	./client;
	rm client
clean:
	rm client
