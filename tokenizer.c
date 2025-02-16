#define MAX_LINE_LENGTH 2048

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * TODO:
 * 0. [x] define data structures
 * 1. [x] load markdown files
 * 2. [x] send it to block tokenizer function line by line
 * 3. [ ] check if given line is block element
 * 4. [ ] if given block element has string, send it to inline tokenizer function
 * 5. [ ] if given string has inline element, tokenize it
*/

typedef enum {
	NONE = 0,
	HEADING1,
	HEADING2,
	HEADING3,
	CODE_BLOCK,
	LINE,
	ORDERED_LIST,
	UNORDERED_LIST,
	PARAGRAPH,
} block_element_type;

typedef enum {
	NORMAL_STRING = 0,
	ITALIC,
	BOLD,
	ITALIC_AND_BOLD,
	INLINE_CODE,
	LINK,
	IMAGE_LINK

} inline_element_type;

typedef struct {
	block_element_type type;
	char* value;
	char* code_type;
	inline_element* in_el;
} block_element;

typedef struct {
	inline_element_type type;
	char* value;
} inline_element;

// ******************************

FILE* load_markdown(char* file_dir);
void process_block();
void process_inline();

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(std_err, "Usage: %s <.md file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *file;
	file = load_markdown(argv[1]);

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), file)){
		process_block(line);
	}
	//inline_tokenizer(); // inline tokenizer is be preceeded in some block tokenizer.
	
	fclose(file);
	return 0;
}

// ******************************

FILE* load_markdown(char* file_dir) {
	FILE *file = fopen(dir, "r");
	if (!file) {
		perror("Can't locate file\n");
		return EXIT_FAILURE;
	}

	return file;
}

void process_block(char* line) {
	block_element_type state = NONE;

	if (strncmp(line, "# ", 2) == 0) {
		printf("Heading 1 detected\n");
		state = HEADING1;
		line = &line[2];
	}
	else if (strncmp(line, "## ", 3) == 0) {
		printf("Heading 2 detected\n");
		state = HEADING2;
		line = &line[3];
	}
	else if (strncmp(line, "### ", 4) == 0) {
		printf("Heading 3 detected\n");
		state = HEADING3;
		line = &line[4];
	}
	else if (strncmp(line, "```", 3) == 0) {
		printf("Code Block detected\n");
		state = CODE_BLOCK;
		// TODO: implement code block's parsing process and others.
	}
	else if (strncmp(line, "***\n", 4) == 0 || strncmp(line, "---\n", 4) == 0) {
		printf("Line detected\n");
		state = LINE;
	}
	else if (is_digit(line[0]) && line[1] == .) {
		printf("Ordered List detected\n");
		state = ORDERED_LIST;
	}
	else if (strncmp(line, "- ", 2) == 0) {
		printf("Unordered List detected\n");
		state = UNORDERED_LIST;
	}
	else {
		printf("Paragraph detected\n");
		state = PARAGRAPH;
	}
	

}

void process_inline() {

}

