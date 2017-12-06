#include <stdio.h>
#include <string.h>
#include "headers.h"

void change_headers(char * message) {

	int i, j, k, header_size, opcao = 0;
	int searching = 0;

	char header[30];
	char value[1024];
	char new_value[1024];

	char headers[4096];
	char new_headers[4096];
	char body[4096];
	char start_line[256];

	char header_arr[20][30];
	char value_arr[20][1024];

	for (i = 0; i < 256; i++) {
		start_line[i] = message[i];

		if (message[i + 1] == '\r' && message[i + 2] == '\n') {
			strcat(start_line, "\r\n");
			i = i + 3;
			break;
		}
	}

	for (j = 0; i < 4096; i++) {
		headers[j++] = message[i];

		if (message[i + 1] == '\r' && message[i + 2] == '\n' && message[i + 3] == '\r' && message[i + 4] == '\n') {
			strcat(headers, "\r\n\r\n");
			header_size = j;
			break;
		}
	}

	for (i = header_size, j = 0; i < 4096; i++)
	{
		body[j++] = message[i];

		if (message[i] == '\0'){
			body[j] = '\0';
			break;
		}
	}

	/*printf("FRIST LINE: \n%s\n", start_line);
	printf("HEADERS:\n%s\n\n", headers);
	printf("BODY:\n%s\n", body);*/

	for (i = 0, j = 0; i <= header_size; i++)	{
		/*printf("[%d/%d] %c %s: %s\n", i, header_size, headers[i], header, value);*/

		if (searching == 0) {

			if (headers[i] == ':' && headers[i + 1] == ' ') {
				header[j] = '\0';
				strcpy(header_arr[k], header);
				/*printf("H: %s\n", header);*/

				j = 0;
				searching = 1;
				i++;
			}
			else
				header[j++] = headers[i];

		} else {

			if (headers[i] == '\r' && headers[i + 1] == '\n') {
				value[j] = '\0';
				strcpy(value_arr[k++], value);
				/*printf("V: %s\n", value);*/

				j = 0;
				searching = 0;
				i++;
			}
			else
				value[j++] = headers[i];
		}
	}

	while (1) {
		printf("\nEscolha o header que deseja modificar:\n\n");

		for (i = 0; i < k; i++)
		{
			printf("%d) %s: %s\n", i + 1, header_arr[i], value_arr[i]);
		}

		printf("\n0) Terminar\n\nOpcao: ");

		scanf("%d", &opcao);

		if (opcao < 0 || opcao > k)
			continue;
		if (opcao == 0)
			break;
		else {
			printf("\nNovo valor: ");
			scanf("%s", &new_value);
		}

		strcpy(value_arr[opcao - 1], new_value);
	}

	for (i = 0; i < k; i++) {
		strcat(new_headers, header_arr[i]);
		strcat(new_headers, ": ");
		strcat(new_headers, value_arr[i]);
		strcat(new_headers, "\r\n");
	}
	strcat(new_headers, "\r\n\0");

	strcpy(message, start_line);
	strcat(message, new_headers);
	strcat(message, body);

	return;
}