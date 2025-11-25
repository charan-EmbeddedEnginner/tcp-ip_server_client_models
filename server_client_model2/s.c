#include "header.h"

struct sockaddr_in   server_id,client_id;

void client_info(struct sockaddr_in);

int main(int argc, char **argv){

	int server_fd;
	int fd;
	int r=0;
	int len;
	char buf[100];
	char rbuf[200];
	char *p=0;
	FILE *fp=0;

	if(argc!=3){
		printf("useage :  ./server  port_ADDR  IP\n");
		return 1;
	}

	printf("---------------------server.c-------------------\n");


	//socket
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==-1){
		perror("socket");
		return 1;
	}
	perror("socket");


	//bind
	server_id.sin_family     = AF_INET; 
	server_id.sin_port       = htons(atoi(argv[1]));
	server_id.sin_addr.s_addr= inet_addr(argv[2]);

	r = bind(server_fd,(struct sockaddr *)&server_id,sizeof(server_id));
	if(r==-1){
		perror("bind");
		return 1;
	}
	perror("bind");


	//listen
	r=listen(server_fd,5);
	if(r==-1){
		perror("listen");
		return 1;
	}
	perror("listen");


	//accept
	len=sizeof(client_id);
	fd=accept(server_fd,(struct sockaddr *)&client_id,&len);
	if(fd==-1){
		perror("accept");
		return 1;
	}
	perror("accept");


	client_info(client_id);
	

	//inform to client connected properly , provide filename	
	r=write(fd,"filename",strlen("filename")+1);//client is wait until server resposce
	if(r<=0){
		perror("write");
		printf("server  unable to inform to client\n");
		goto END;
	}


	//0.server read filename from client.
	//1.check if file exist,send data to client.
	//2.if not exist,send file not exist.
	

	//readfilename from server
	r=read(fd,&buf,sizeof(buf)); //0.  
	if(r<=0){
		perror("read");
		printf("unable to read filename from client\n");
		goto END;
	}
	printf("read:%d %s \n",r,buf);
	

	fp=fopen(buf,"r"); //check file exist/not
	if(fp==0){
		perror("fopen");
		printf("file not exist\n");
		write(fd,"FN",3);
		goto END;
	}
	else{
		write(fd,"FE",3);
	}


	r=read(fd,buf,sizeof(buf));
	if(r<=0){
		printf("server fail to read client not ok\n");
		goto END;
	}
	printf("r:%d %s\n",r,buf);
	if(strcmp(buf,"NOK")==0){
		printf("client not ready/client side problem\n");
		goto END;
	}


	//file exist
	//send data untill endoffile
	while(1){

		p=fgets(rbuf,200,fp);
		if(p==0){

			fclose(fp);
			fp=0;
			r=write(fd,"EOF",3);
			if(r<=0){
				printf("server unable to send data to client\n");
				goto END;
			}
			printf("%s\n","EOF");
			break;
		}
		else{

			//send data to client
			r=write(fd,rbuf,strlen(rbuf));
			if(r<=0){
				printf("server unable to send data to client\n");
				goto END;
			}
			printf("sent r:%d %s",r,rbuf);
			
			/*ack
			r=read(fd,rbuf,sizeof(rbuf));//response from client
			if(r<=0){
				perror("read ack");
				break;
			}
			rbuf[r]='\0';
			printf("\tack r:%d %s\n",r,rbuf);
			*/
		}
	
	}
	//if reach to here
	//file transmitted sucessfullly wait untill client read,client send ack at end	      
	r= read(fd,rbuf,strlen("ACK")+1);
	if(r<=0){
		printf("client unable to send ack at end of file\n");

	}
	else{
	   rbuf[r]=0;
	   if(strcmp("ACK",rbuf)==0)
		printf("file transmitted successfully.\n");
	   else
		printf("client not send ack correctly\n");
	}
	

END:
	if(fp)
		fclose(fp);

	close(fd);
	close(server_fd);
	
	return 0;
}


void client_info(struct sockaddr_in client_id){
	
	printf("\nserver_data------\n");
	printf("server port:%d\n",ntohs(server_id.sin_port));
	printf("server_ip  :%s\n",inet_ntoa(server_id.sin_addr));

	
	printf("\nclient_data:------\n");
	printf("client port:%d\n",ntohs(client_id.sin_port));
	printf("client_ip  :%s\n",inet_ntoa(client_id.sin_addr));

}

