
.PHONY:all

all:tcp_pthread tcp_client

tcp_pthread:tcp_pthread.c
	gcc -o $@ $^ -lpthread
tcp_client:tcp_client.c
	gcc -o $@ $^
.PHONY:clean

clean:
	rm -f tcp_pthread tcp_client
