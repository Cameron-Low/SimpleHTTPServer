#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htmlFileParser.h"

char* parseFile(FILE *f) {
	char *document = malloc(sizeof(char)*LINE_WIDTH);
	char chr;
	int n = 0;
	
	while ((chr = fgetc(f)) != EOF) {
			document[n++] = chr;
	}
	return document;
}

