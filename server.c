#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "htmlFileParser.h"
#include "server.h"

int main() {
	// Create socket for TCP
	int serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketFd == 0) {
		fprintf(stderr, "Socket creation failed\n");
		return -1;
	}

	// Setup sockaddr struct
	struct sockaddr_in socketAddress;

	// Setting contents to 0
	memset((char *) &socketAddress, 0, sizeof(socketAddress));

	const int PORT = 8080;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(PORT);
	socketAddress.sin_addr.s_addr = htonl(INADDR_ANY); 

	// Bind socket
	if (bind(serverSocketFd, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0) {
		fprintf(stderr, "Socket binding failed\n");
		return -1;
	}

	// Listen
	if (listen(serverSocketFd, 10) < 0) {
		fprintf(stderr, "Listening failed\n");
		return -1;
	}

	for (;;) {
		printf("Waiting...\n");
		
		// Open connection to client
		struct sockaddr clientAddress;
		socklen_t sockStructLength;
		int client_socket = accept(serverSocketFd, &clientAddress, &sockStructLength); 
		
		if (client_socket < 0) {
			fprintf(stderr, "Accepting failed, %d\n",client_socket);
			return -1;
		}
		
		// Handle client request
		char *buff = malloc(sizeof(*buff) * MAX_BYTESTREAM);
		read(client_socket, buff, MAX_BYTESTREAM);
		
		struct HTTPRequest request;
		parseRequest(&request, buff);

/*		for (int i  = 0; i < MAX_HEADERS ; i++) {
			if (request.headers[i].name == NULL) break;
			printf("%s: ", request.headers[i].name);
			printf("%s\n", request.headers[i].value);
		}

*/		
		char *response = "HTTP/1.1 200 Ok\nServer: Cameron\nContent-Type: text/html\nContent-Length: ";

		// Send the file
		FILE *file = fopen(request.line.requestTarget, "r");
		char *doc = parseFile(file);
		char len[4];
	   	
		char *httpResp = malloc(sizeof(*httpResp) * (strlen(doc) + strlen(response) + 10));
		strcpy(httpResp, response);

		sprintf(len, "%lu", strlen(doc)+1);
		strcat(httpResp, len);
		strcat(httpResp, "\n");
		strcat(httpResp, "\n");
		strcat(httpResp, doc);
		strcat(httpResp, "\n");
		write(client_socket, httpResp, strlen(httpResp));
		
		close(client_socket);
	}
}

void parseRequest(struct HTTPRequest *r, char *buf) {
	// Split into lines based off of \n
	char *b = buf;
    char* line;
	
	char* lines[MAX_REQUEST_LINE_LENGTH];
    line = strsep(&buf, "\n");
	int n = 0;
	while (line != NULL) {
		lines[n++] = line;
		line = strsep(&buf, "\n");
	}
	lines[n] = line;
	free(b);

	// Parse request line
	char *requestLine = lines[0];
	
	char *method = strsep(&requestLine, " ");
	if (strcmp(method, "GET")) {
		r->line.method = GET;
	} else {
		r->line.method = UNSUPPORTED;
	}
	r->line.requestTarget = strsep(&requestLine, " ");
	r->line.requestTarget++;
	r->line.version = requestLine;

	// Parse headers
	int m = 1;
	while (strcmp(lines[m], "\r") != 0 && strcmp(lines[m], "\n") != 0) {
		char *headerLine = lines[m];
		struct HTTPHeader header;
		header.name = strsep(&headerLine, ":");
		
		// Clean up header value	
		char *end;
		while(isspace((unsigned char)* headerLine)) headerLine++;
		end = headerLine + strlen(headerLine) - 1;
		while(end > headerLine && isspace((unsigned char)*end)) end--;
	  	*(end+1) = 0;

		header.value = headerLine;
		r->headers[m-1] = header;
		m++;
	}

	// Parse message
	char *l = malloc(sizeof(*l) * MAX_REQUEST_LINE_LENGTH);
	char *msg = l;
	for (int i = m; i < n; i++) {
		memcpy(l, lines[i], strlen(lines[i]));
        l += strlen(lines[i]);
	}
	*l = '\0';
	r->message = msg;
}
