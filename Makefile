all: server client
server:UdpServer.cpp
	g++ -g $^ -o $@ -lpthread
client:UdpClient.cpp
	g++ $^ -o $@

.PHONY:clean
clean:
	-rm server client