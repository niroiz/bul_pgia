#include <unistd.h>          
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>         
#include <signal.h>    
#include <sys/un.h>
#include <netdb.h>

#define SIM_LENGTH 10 
#define IP_ADDRESS "127.0.0.1" 
#define PORT 1339 
#define SIZE 5
#define SIZE_OF_ROUNDS 10


int main(int argc, char* argv[])
{ 
  int sock;   /* hold socket file descriptor */
  struct sockaddr_in cli_name;  /* hold destination details (IP+PORT) */
  struct addrinfo* res;
  struct sockaddr_in* saddr;
  char *hostname, *hostaddr;
  size_t len;
  char msg[SIZE];
  char answer[180];
  int i;

  hostname = argv[1];

  if (argc != 2)
  {
	perror("Usage: hostnamelookup <hostname>\n");
	exit(1);
  }	


  printf("Client is alive and establishing socket connection.\n");
  

  if (0 != getaddrinfo(hostname, NULL, NULL, &res)) /* get ip from DNS */
  {
  	fprintf(stderr, "Error in resolving hostname %s\n", hostname);
  	exit(1);
  }


 	saddr = (struct sockaddr_in*)res->ai_addr;
	hostaddr = inet_ntoa(saddr->sin_addr);  /* hold the ip */

  sock = socket(AF_INET, SOCK_STREAM, 0);  /* make a socket */

  if (sock < 0)  /* validate the socket */
    { perror ("Error opening channel");
      close(sock);
      exit(1);
    }



  bzero(&cli_name, sizeof(cli_name)); /* reset all */
  cli_name.sin_family = AF_INET;  /* setting the protocol – IPv4 */
  cli_name.sin_addr.s_addr = inet_addr(hostaddr);  /* setting the address */
  cli_name.sin_port = htons(PORT);   /* setting the port */



  if (connect(sock, (struct sockaddr *)&cli_name, sizeof(cli_name)) < 0)  /* connecting to server and validating connection */
    { perror ("Error establishing communications");
      close(sock);
      exit(1);
    }


  for (i = 0; i < SIZE_OF_ROUNDS; i++){   
	if((!strcmp(answer, "\nGood job, You won! :)\n\n")) || (!strcmp(answer, "\nGame over, You lose! :(\n\n"))){
		break;
	}

	read(sock, answer, 180);	
	printf("\n%s\n", answer);
	printf("Write your guess: ");
	scanf("%s", msg);
	write(sock, msg, SIZE);
	read(sock, answer, 180);
	printf("\n%s", answer);

	while(!strcmp(answer, "Invalid input. try again, guess a 4 numbers: ")){
		scanf("%s", msg);
		write(sock, msg, SIZE);
		read(sock, answer, 180);
		printf("\n%s", answer);		
	}
	
    }


  printf("Exiting now.\n");
  close(sock);  /* closes socket */
  exit(0); 

} 
