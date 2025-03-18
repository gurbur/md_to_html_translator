#define DEFAULT_FILE_NAME "output.json"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "list_head.h"
#include "types.h"

/*
 * TODO:
 * 0. [x] define data structures
 * 1. [x] load .tokens files
 * 2. [ ] parse file into data type
 * 3. [ ] check if given line is block element
 * 4. [ ] if given block element has string, send it to inline tokenizer function
 * 5. [ ] if given string has inline element, tokenize it
 * 6. [ ] print the output into specific file format
*/


int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "[ERROR] Usage: %s <.tokens file> [-o <output file name>]\n", argv[0]);
		return EXIT_FAILURE;
	}
	char* input_dir = argv[1];
	char* output_dir = DEFAULT_OUTPUT;

	// check output directory option
	for(int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
			output_dir = argv[i + 1];		
			i++;
		}
	}

	FILE *input;
	input = load_file(input_dir, "r");
	
	if (input == NULL) {
		return EXIT_FAILURE;
	}

	FILE *output;
	output = load_file(output_dir, "w");
	
	if (input == NULL) {
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), input)){
		parse_line(line);
	}
	
	fclose(input);
	fclose(output);

	printf("[COMPLETE] End of Parsing: %s -> %s\n", input_dir, output_dir);
	return EXIT_SUCCESS;

	return 0;
}

FILE* load_file(char* file_dir, char* mode) {
	FILE *file = fopen(file_dir, mode);
	if (!file) {
		if (mode[0] == 'r') {
			perror("[ERROR] Can't locate file\n");
			return NULL;
		}
		else if (mode[0] == 'w') {
			perror("[ERROR] Can't create output file\n");
			return NULL;
		}
	}

	return file;
}


