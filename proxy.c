#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <unistd.h>

char *hostname;

void filtrarRequisicoes(){
    
    size_t len = 0;
    char linha [1024];
    ssize_t chars;
    
    int whitelisted = 0;
    
    /*Abrir arquivo de whitelist*/
    
    FILE *whitelist;
    
    whitelist = fopen("whitelist.txt", "r");
    
    
    /*Percorrendo linha por linha do arquivo*/
    while (fgets(linha, sizeof(linha), whitelist) != NULL)
    {
        /*Tirando \n caso haja*/
        if (linha[strlen(linha) - 1] == '\n')
            linha[strlen(linha) - 1] = '\0';
        
        /*Se arquivo estiver na whitelist, printar*/
        if(strcmp(hostname, linha) == 0){
            printf("\nEndereço %s está whitelisted.\n", hostname);
            whitelisted = 1;
        }
        
    }
    
    fclose(whitelist);
    
    /*Caso já tenha sido whitelisted, ignorar próximos passos*/
    if(whitelisted != 1){

        FILE *blacklist;
        
        blacklist = fopen("blacklist.txt", "r");

        
        /*Percorrendo linha por linha do arquivo*/
        while (fgets(linha, sizeof(linha), blacklist) != NULL)
        {
            /*Tirando \n caso haja*/
            if (linha[strlen(linha) - 1] == '\n')
                linha[strlen(linha) - 1] = '\0';
            
            /*Se arquivo estiver na blacklist, printar*/
            if(strcmp(hostname, linha) == 0){
                printf("\nEndereço %s está blacklisted.\n", hostname);
            }
            
        }
        
        fclose(blacklist);
        
        
    }
    
}

int main() {

	int BUFFER_SIZE = 4096;

	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9874);
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	listen(server_socket, 5);

	int client_socket;
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);

	while (1)
	{
        
		client_socket = accept(server_socket, (struct sockaddr*) &client_address, &addrlen);

		char client_ip[addrlen];
		inet_ntop(AF_INET, &client_address, client_ip, addrlen);
		printf("Client IP: %s\n", client_ip);

		char client_message[1024];
		char client_message_copy[1024];

		recv(client_socket, client_message, BUFFER_SIZE , 0);
		printf("Client Request:\n%s\n", client_message);

		char * pch;
		char * method;
		/*char * hostname;*/

		strcpy(client_message_copy, client_message);

		pch = strtok(client_message_copy, " :\n\r");
		while (pch != NULL)
		{
			if (strcmp(pch, "GET") == 0) {
				method = malloc(strlen(pch) + 1);
				strcpy(method, pch);
			}

			if (strcmp(pch, "Host:") == 0) {
				hostname = malloc(strlen(pch));
				pch = strtok (NULL, " :\n\r");
				strcpy(hostname, pch);
				break;
			}

			pch = strtok (NULL, " \n");
		}

		printf ("Method: %s\n", method);
		printf ("Hostname: %s\n", hostname);
        
        filtrarRequisicoes();


		struct hostent *he = gethostbyname(hostname);

		if (he == NULL) {
			printf("Erro ao obter IP do host pelo nome\n");
			return (0);
		}

		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, he->h_addr_list[0], ipstr, sizeof(ipstr));
		printf("Endereco: %s\n\n", ipstr);
        
		int remote_socket = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in remote_address;
		remote_address.sin_family = AF_INET;
		remote_address.sin_port = htons(80);
		inet_aton(ipstr, &remote_address.sin_addr.s_addr);

		if (connect(remote_socket, (struct sockaddr *) &remote_address, sizeof(remote_address)) < 0) {
			printf("Erro ao conectar ao socket remoto\n");
		}

		send(remote_socket, client_message, sizeof(client_message), 0);

		ssize_t n;
		char response[BUFFER_SIZE];

		while ((n = recv(remote_socket, response, BUFFER_SIZE, 0)) > 0) {
			send(client_socket, response, n, 0);
		}

		/*recv(remote_socket, &response, sizeof(response), 0);*/
		/*printf("Remote response:\n%s\n\n", response);*/
		/*send(client_socket, response, sizeof(response), 0);*/

	}

	close(server_socket);

	return 0;
}
