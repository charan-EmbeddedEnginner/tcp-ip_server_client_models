#include "header.h"

int main(int argc, char **argv){

struct sockaddr_in server_id;
int fd;
int r=0;
int len;
int lr=0;
char rbuf[100];
char buf[25];
char filename[50];
FILE *fp=0;

	if(argc!=4){
		printf("useage : ./client   server_PORTADDR   server_IP destination_filename\n");
		return 1;
	}


	printf("----------------client-------------------\n");


	//socket
	fd=socket(AF_INET,SOCK_STREAM,0);
	if(fd==-1){
		perror("socket");
		return 1;
	}
	perror("socket");


	//connect
	server_id.sin_family	 = AF_INET;
	server_id.sin_port  	 = htons(atoi(argv[1]));
	server_id.sin_addr.s_addr= inet_addr(argv[2]);

	r=connect(fd,(struct sockaddr *)&server_id,sizeof(server_id));
	if(r<0){
		perror("connect");
		return 1;
	}
	perror("connect");
	
	
	printf("\nserver_data------\n");
	printf("server port:%d\n",ntohs(server_id.sin_port));
	printf("server_ip  :%s\n",inet_ntoa(server_id.sin_addr));



	//wait until server respond
	r=read(fd,buf,sizeof(buf));
	if(r<=0){
		perror("read");
		printf("client unable to read server sent filename\n");
		goto END;
	}
	if(strcmp("filename",buf)){
		printf("server not connected properly ,reconnect\n");
		goto END;
	}



	//if server respond properly send filename to server
	printf("enter filename:");
	scanf("%s",filename);
		

	//send filename to server
	r=write(fd,filename,strlen(filename)+1);
	if(r<=0){
		printf("unable to snd filename to server\n");
		goto END;
	}
	printf("r:%d %s\n",r,filename);


	//check file_status	
	r=read(fd,buf,sizeof(buf));
	if(r<=0){
		printf("unable read file status from server\n");
		goto END;
	}
	printf("r:%d %s \n",r,buf);
	if(strcmp(buf,"FN")==0){ //if filenot exist in server
		printf("file not exist/invalid file in server\n");
		goto END;		
	}
	

	//if file exist
	fp=fopen(argv[3],"w");
	if(fp==0){
		write(fd,"Nok",4);//response to server
		perror("fopen");
		goto END;
	}
	r=write(fd,"ok",3);//response to server
	if(r<=0){
		printf("unable to snd response to server\n");
		goto END;
	}


	while(1){
		r=read(fd,rbuf,sizeof(rbuf));
		if(r<=0){
			perror("read");
			printf("client unable to read data from server\n");
			break;
		}
		rbuf[r]=0;
		printf("r:%d %s\n",r,rbuf);

		if(!strcmp(rbuf,"EOF")){
			fclose(fp);
			fp=0;
			printf("file received\n");
			break;
		}
		else{
			fputs(rbuf,fp);
		}
		
		//ack
		r=write(fd,"ok",3);	
		if(r<=0){
			printf("unable to snd filename to server\n");
			break;
		}
	}


END:
	if(fp){
		fclose(fp);
		printf("terminated unconditionally\n");
	}

	close(fd);
	return 0;
}

