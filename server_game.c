#include <sys/types.h>
#include <unistd.h>          
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>     
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>


#define SIM_LENGTH 10 
#define PORT 1339
#define SIZE 5
#define SIZE_OF_ROUNDS 10

int ReturnBull(char guess[], char code[]);
int ReturnPgia(char guess[], char code[]);
void GenerateCode(char code[]);
int validateGuess(char guess[]);



void clean_up(int cond, int *sock)
{ printf("Exiting now.\n");
  close(*sock); 
  exit(cond);
} 


int main(void)
{ 
  int sock;  /* hold socket file descriptor */
  int connect_sock;  /* holds new socket file descriptor */
  struct sockaddr_in serv_name; /* holds local details (IP+PORT) */
  size_t len; 
  int count;
  int i;
  char answer[SIZE];
  char msg[180];
  char numrand[SIZE];
  int pgia, bull;
  

  sock = socket(AF_INET, SOCK_STREAM, 0);  /* make a socket */

  if (sock < 0)  /* validate the socket */
    { perror ("Error opening channel");
      clean_up(1, &sock);
    }

  
  bzero(&serv_name, sizeof(serv_name));  /* reset all */
  serv_name.sin_family = AF_INET;      /* setting the protocol – IPv4 */                          
  serv_name.sin_port = htons(PORT);    /* setting the port */
  serv_name.sin_addr.s_addr = htonl(INADDR_ANY);  /* setting the adress */


  if (bind(sock, (struct sockaddr *)&serv_name, sizeof(serv_name)) < 0)  /* binding the socket with the specific IP+PORT */
    { perror ("Error naming channel");
      clean_up(1, &sock);
    }

      
  printf("\nServer is alive and waiting for socket connection from client.\n");
  listen(sock, 1);  /* listening for connection request */



  len = sizeof(serv_name);
  connect_sock = accept(sock, (struct sockaddr *)&serv_name, &len);  /* accepting a connection request */


GenerateCode(numrand);
puts(numrand);


  for (i = 0; i < SIZE_OF_ROUNDS; i++)	
    { 
	sprintf(msg, "\n----Round Number %d----\n\nGuess a %d numbers from 0 to 9\n", i+1, SIZE-1);
	write(connect_sock, msg, 180);	
	read(connect_sock, answer, SIZE);
	
	while(validateGuess(answer)){
	sprintf(msg, "Invalid input. try again, guess a 4 numbers: ");
	write(connect_sock, msg, 180);	
	read(connect_sock, answer, SIZE);
	}
	
	pgia = ReturnPgia(answer, numrand);
	bull = ReturnBull(answer, numrand);
	if(bull == 4){
		sprintf(msg, "\nGood job, You won! :)\n\n");
		write(connect_sock, msg, 180);
		break;	
	}
	if (i == 9 && bull != 4){
		sprintf(msg, "\nGame over, You lose! :(\n\n");
		write(connect_sock, msg, 180);
		break;
	}	
	sprintf(msg, "Your Bulls is: %d\nYour Pgiot is: %d\n\n----------------------\n", bull, pgia);
	write(connect_sock, msg, 180);
	
    }



  close(connect_sock);  /* closes socket */
  clean_up(0, &sock); 
}




int ReturnBull(char guess[], char code[]){

	char *pcode = code, *pguess = guess;
	int i, count = 0;

	for (i = 0; i<SIZE - 1; i++)
	{
		if (*(pcode + i) == *(pguess + i))
			count++;
	}

	return count;
}


int ReturnPgia(char guess[], char code[]){

	char *pcode = code, *pguess = guess;
	int i, j, count = 0;
	
	for (i = 0; i<SIZE - 1; i++)
	{
		for (j = 0; j<SIZE - 1; j++)
		{
			if (*(pcode + i) == *(pguess + j))
			{
				count++;
				break;
			}
		}
	}
	count -= ReturnBull(code, guess);

	return count;

}


void GenerateCode(char code[])
{
	char *ptr = code;
	int i, j;
	
	for (i = 0; i<SIZE - 1;)
	{
		srand(time(0));
		*(ptr + i) = rand() % 10 + '0';
		for (j = 0; j<i;)
		{
			if (*(ptr + i) != *(ptr + j))
				j++;
			else
				break;
		}
		if (j == i)
			i++;
	}
	*(ptr + i) = NULL;
}


int validateGuess(char guess[])
{
	char *ptr = guess;
	int i, valid = 0;

	if (*(ptr + SIZE - 1) != NULL)
		valid = 1;
	else
	{
		for (i = 0; i<SIZE - 1; i++)
		{
			if (*(ptr + i)<'0' || *(ptr + i)>'9')
			{
				valid = 1;
				break;
			}
		}
	}

	return valid;
}



