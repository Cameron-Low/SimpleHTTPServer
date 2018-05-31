#ifndef server
#define server

#define MAX_BYTESTREAM 2000
#define MAX_REQUEST_LINE_LENGTH 20 
#define MAX_HEADERS 20

// HTTP Request structures

enum Method {
	GET,
	UNSUPPORTED
};

struct RequestLine {
	enum Method method;
	char *requestTarget;
	char *version;
};

struct HTTPHeader {
	char* name;
	char* value;
};

struct HTTPRequest {
	struct RequestLine line;
	struct HTTPHeader headers[MAX_HEADERS];
	char *message;
};

void parseRequest(struct HTTPRequest *request, char *buf);

#endif
