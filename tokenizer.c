#define MAX_LINE_LENGTH 2048

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
 * TODO:
 * 0. [x] define data structures
 * 1. [x] load markdown files
 * 2. [x] send it to block tokenizer function line by line
 * 3. [x] check if given line is block element
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
	struct inline_element* in_el;
} block_element;

typedef struct {
	inline_element_type type;
	char* value;
} inline_element;

// ******************************

block_element_type block_state = NONE;


FILE* load_markdown(char* file_dir);
void process_block(char* line);
void process_inline();

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "[ERROR] Usage: %s <.md file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *file;
	file = load_markdown(argv[1]);
	
	if (file == NULL) {
		return EXIT_FAILURE;
	}

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
	FILE *file = fopen(file_dir, "r");
	if (!file) {
		perror("[ERROR] Can't locate file\n");
		return NULL;
	}

	return file;
}

void process_block(char* line) {
	bool skip_flag = false;
	if (block_state == CODE_BLOCK) {
		if (strncmp(line, "```", 3) == 0) {
			printf("End of Code Block\n");
			block_state = NONE;
		}
		else {
			printf("Keep processing Code Block...\n");
			skip_flag = true;
		}
	}
	else if (block_state == ORDERED_LIST) {
		if (isdigit(line[0]) && strncmp(&line[1], ". ", 2) == 0) { // edge case: what if there is 2 or more digits? (ex. 13, 143, ...)
			printf("Keep processing Ordered List(%c)...\n", line[0]);
			skip_flag = true;
			line = &line[3];
		}
		else {
			printf("End of Ordered List\n");
			block_state = NONE;
		}
	}
	else if (block_state == UNORDERED_LIST) {
		if (strncmp(line, "- ", 2) == 0) {
			printf("Keep processing Unordered List...\n");
			skip_flag = true;
			line = &line[2];
		}
		else {
			printf("End of Unordered List\n");
			block_state = NONE;
		}
	}
	if (skip_flag) {
		// do_nothing();
	}
	else if (strncmp(line, "# ", 2) == 0) {
		printf("Heading 1 detected\n");
		block_state = HEADING1;
		line = &line[2];
	}
	else if (strncmp(line, "## ", 3) == 0) {
		printf("Heading 2 detected\n");
		block_state = HEADING2;
		line = &line[3];
	}
	else if (strncmp(line, "### ", 4) == 0) {
		printf("Heading 3 detected\n");
		block_state = HEADING3;
		line = &line[4];
	}
	else if (strncmp(line, "```", 3) == 0) {
		printf("Code Block detected\n");
		block_state = CODE_BLOCK;
		if (line[3] != '\n') {
			printf("Code type is %s", &line[3]);
		}
	}
	else if (strncmp(line, "***\n", 4) == 0 || strncmp(line, "---\n", 4) == 0) {
		printf("Line detected\n");
		block_state = LINE;
	}
	else if (isdigit(line[0]) && strncmp(&line[1], ". ", 2) == 0) {
		printf("Ordered List detected\n");
		block_state = ORDERED_LIST;
		line = &line[3];
	}
	else if (strncmp(line, "- ", 2) == 0) {
		printf("Unordered List detected\n");
		block_state = UNORDERED_LIST;
		line = &line[2];
	}
	else if (block_state == PARAGRAPH) {
		printf("Keep processing Paragraph...\n");
	}
	else {
		printf("Paragraph detected\n");
		block_state = PARAGRAPH;
	}
	
	if (block_state == HEADING1 || block_state == HEADING2 ||\
		block_state == HEADING3 || block_state == ORDERED_LIST ||\
		block_state == UNORDERED_LIST || block_state == PARAGRAPH)
	{
		process_inline(line);
	}

}

void process_inline(char* line) {
	printf("inline processing: %s", line);
}

