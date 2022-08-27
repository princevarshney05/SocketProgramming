#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include<stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8080 
int BLOCK_SIZE = 1024;

float calculate_percentage(int amount_read,int File_length);


int main(){
    int client_socket;
    struct sockaddr_in address;
    if((client_socket = socket(AF_INET,SOCK_STREAM,0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if(inet_pton(AF_INET,"127.0.0.1",&address.sin_addr)<=0){
        perror("Problem in address translation");
        exit(EXIT_FAILURE);
    }
    if(connect(client_socket,(struct sockaddr *)&address,sizeof(address)) < 0){
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }
    while(1){
        char *input = readline("client>");
        
        char **argv = (char **)malloc(1000*sizeof(char *));
        char *current;
        int i=0,number_of_files;
        while((current = strtok(input," ")) != NULL){
            argv[i] = current;
            i++;
            input = NULL;
        }
        if(i == 0){
            continue;
        }

        if(strcmp(argv[0],"exit") == 0){
            write(client_socket,argv[0],strlen(argv[0]));
            break;
        }
        else if(strcmp(argv[0],"get") == 0){
            number_of_files = i - 1;
            if(number_of_files == 0){
                printf("Usage get <file/s>\n");
                continue;
            }

            for(int j=1;j<=number_of_files;j++){
                write(client_socket,argv[j],strlen(argv[j]));
                char buffer[100],FileData[BLOCK_SIZE];
                bzero(buffer,100);
                read(client_socket,buffer,100);
                printf("Message from server : %s\n",buffer);
                int FileLength = atoi(buffer);
                int amount_read = 0;
                if(strcmp(buffer,"File Not Found") == 0){
                    continue;
                }
                else if(buffer[0] == '\0'){
                    printf("No response from server\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    
                    
                    int fd = open(argv[j],O_WRONLY | O_CREAT,0777);
                    while(calculate_percentage(amount_read,FileLength) < 100){
                        write(client_socket,"give",strlen("give"));  //send the block
                        if(FileLength - amount_read < BLOCK_SIZE){
                            BLOCK_SIZE = FileLength - amount_read;
                        }
                        bzero(FileData,BLOCK_SIZE);
                        read(client_socket,FileData,BLOCK_SIZE);
                        write(fd,FileData,BLOCK_SIZE);
                        amount_read += BLOCK_SIZE;
                        printf("Downloading %s : %.2f\r",argv[j],calculate_percentage(amount_read,FileLength));
                        fflush(stdout);
                    }
                    printf("File Sucesfully Downloaded !!!!!\n");
                    
                }
                
            }
        
            
        }
        else{
            printf("Invalid Commands\n");
        }
       
        

    }
    close(client_socket);

}

float calculate_percentage(int amount_read,int File_length){
    
    return ((float)amount_read/File_length)*100;
}