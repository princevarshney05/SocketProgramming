#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8080
int BLOCK_SIZE  = 1024;


float calculate_percentage(int amount_read,int File_length);

int main(){
    int server_socket,addr_len,client_socket,opt=1;
    struct sockaddr_in address;
    
    addr_len = sizeof(address);
    if((server_socket = socket(AF_INET,SOCK_STREAM,0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_socket,(struct sockaddr *)&address,sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if(listen(server_socket,3)<0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if((client_socket = accept(server_socket,(struct sockaddr *)&address,(socklen_t *)&addr_len)) < 0){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
            
    char buffer[100],buffer1[100],buffer2[1024],FileData[BLOCK_SIZE];
    while(1){
        bzero(buffer,100);
        read(client_socket,buffer,100);
        if(strcmp(buffer,"exit") == 0){
            
            break;
        }
        else{
            int fd = open(buffer,O_RDONLY,0777);
            if(fd != -1){
                int FileLength = lseek(fd,0,SEEK_END);
                
                bzero(buffer1,100);
                sprintf(buffer1,"%d",FileLength);
                write(client_socket,buffer1,strlen(buffer1));
                int amount_read = 0;
                while(calculate_percentage(amount_read,FileLength) < 100){
                    bzero(buffer2,1024);
                    read(client_socket,buffer2,1024); //client telling ready to recieve the block
                    if(FileLength - amount_read < BLOCK_SIZE){
                        BLOCK_SIZE = FileLength - amount_read;
                    }
                    lseek(fd,amount_read,SEEK_SET);
                    bzero(FileData,BLOCK_SIZE);
                    read(fd,FileData,BLOCK_SIZE);
                    write(client_socket,FileData,BLOCK_SIZE);
                    amount_read += BLOCK_SIZE;
                }
                
                
                
                
            }
            else{
                write(client_socket,"File Not Found",strlen("File Not Found"));

            }
        }
    }
    
    
     
            
            
            
        
    
    
    
    
    
    return 0;


}

float calculate_percentage(int amount_read,int File_length){
    
    return ((float)amount_read/File_length)*100;
}