#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
void Usage()
{
	printf("usage: [ip] [port]");
}
typedef struct Arg
{
	int fd;
	struct sockaddr_in addr;
}Arg;

void ProcessRequest(int client_fd,struct sockaddr_in* client_addr)
{
	char buf[1024] = {0};
	while(1)
	{
		ssize_t read_size = read(client_fd,buf,sizeof(buf));
		if(read_size<0)
		{
			perror("read");
			continue;
		}
		if(read_size==0)
		{
			printf("client: %s bye\n",inet_ntoa(client_addr->sin_addr));
			close(client_fd);
			break;
		}
		buf[read_size]='\0';
		printf("client: %s : %s\n",inet_ntoa(client_addr->sin_addr),buf);
		write(client_fd,buf,sizeof(buf));
	}
	return;
}

void* CreateWorker(void* ptr)
{
	Arg* arg = (Arg*)ptr;
	ProcessRequest(arg->fd,&arg->addr);
	free(arg);
	return NULL;
}

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		Usage();
		return 1;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(atoi(argv[2]));
	//创建套接字
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd<0)
	{
		perror("socket");
		return 2;
	}
	//绑定套接字
	if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))<0)
	{
		perror("bind");
		return 3;
	}
	if(listen(fd,10)<0)
	{
		perror("listen");
		return 4;
	}
	while(1)
	{
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		int client_fd = accept(fd,(struct sockaddr*)&client_addr,&len);
		if(client_fd<0)
		{
			perror("accept");
			continue;
		}
		pthread_t tid = 0;
		Arg* arg = (Arg*)malloc(sizeof(Arg));
		arg->fd = client_fd;
		arg->addr = client_addr;
		pthread_create(&tid,NULL,CreateWorker,(void*)arg);
		pthread_detach(tid);
	}
	return 0;	
}
