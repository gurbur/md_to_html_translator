#define MAX_LINE_LENGTH 2048
#define MAX_INLINE_BUFFER 100

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
 * 4. [x] if given block element has string, send it to inline tokenizer function
 * 5. [x] if given string has inline element, tokenize it
*/

typedef enum {
	NONE_BLOCK = 0,
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
	NONE_INLINE = 0,
	NORMAL_STRING,
	ITALIC,
	BOLD,
	ITALIC_AND_BOLD,
	INLINE_CODE,
	LINK,
	LINK_SHOWN,
	LINK_HIDDEN,
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

block_element_type block_state = NONE_BLOCK;


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
			block_state = NONE_BLOCK;
			skip_flag = true;
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
			block_state = NONE_BLOCK;
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
			block_state = NONE_BLOCK;
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
	inline_element_type inline_state = NONE_INLINE;
	printf("inline processing: %s", line);
	int i = 0;
	char buffer[MAX_INLINE_BUFFER];

	while(line[i] != '\0') {
		bool skip_flag = false;
		if (inline_state == ITALIC) {
			if (line[i] == '*') {
				printf("\nEnd of Italic\n");
				inline_state = NONE_INLINE;
				i += 1;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == BOLD) {
			if (line[i] == '*' && line[i + 1] == '*') {
				printf("\nEnd of Bold\n");
				inline_state = NONE_INLINE;
				i += 2;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == ITALIC_AND_BOLD) {
			if (line[i] == '*' && line[i + 1] == '*' && line[i + 2] == '*') {
				printf("\nEnd of Italic and Bold\n");
				inline_state = NONE_INLINE;
				i += 3;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == INLINE_CODE) {
			if (line[i] == '`') {
				printf("\nEnd of Inline Code\n");
				inline_state = NONE_INLINE;
				i += 1;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == IMAGE_LINK) {
			if (line[i] == ']' && line[i + 1] == ']') {
				printf("\nEnd of Image Link\n");
				inline_state = NONE_INLINE;
				i += 2;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == LINK) {
			if (line[i] == ']' && line[i + 1] == ']') {
				printf("\nEnd of Link\n");
				inline_state = NONE_INLINE;
				i == 2;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == LINK_SHOWN) {
			if (line[i] == ']' && line[i + 1] == '(') {
				printf("\nEnd of Showing Link\nHidden Link: ");
				inline_state = LINK_HIDDEN;
				i += 2;
				skip_flag = true;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == LINK_HIDDEN) {
			if (line[i] == ')') {
				printf("\nEnd of Hidden Link\n");
				inline_state = NONE_INLINE;
				i += 1;
			}
			else {
				printf("%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}

		if (skip_flag) {
			//do_nothing();
		}
		else if (line[i] == '\0') {
			break;
		}
		else if (line[i] == '*' && line[i + 1] == '*' && line[i + 2] == '*') {
			printf("\nItalic and Bold Detected: ");
			inline_state = ITALIC_AND_BOLD;
			i += 3; // skip ***
		}
		else if (line[i] == '*' && line[i + 1] == '*') {
			printf("\nBold Detected: ");
			inline_state = BOLD;
			i += 2;
		}
		else if (line[i] == '*') {
			printf("\nItalic Detected: ");
			inline_state = ITALIC;
			i += 1;
		}
		else if (line[i] == '`') {
			printf("\nInline Code Detected: ");
			inline_state = INLINE_CODE;
			i += 1;
		}
		else if (line[i] == '!' && line[i] == '[' && line[i] == '[') {
			printf("\nImage Link Detected.\n");
			inline_state = IMAGE_LINK;
			i += 3;
		}
		else if (line[i] == '[' && line[i + 1] == '[') {
			printf("\nLink Detected: ");
			inline_state = LINK;
			i += 2;
		}
		else if (line[i] == '[') {
			printf("\nHidden/Shown Link Detected.\n");
			inline_state = LINK_SHOWN;
			i += 1;
		}
		else if (line[i] == '\\') {
			printf("\nEscape Character Detected: %c", line[i + 1]);
			inline_state = NORMAL_STRING;
			i += 2;
		}
		else if (inline_state == NORMAL_STRING) {
			printf("%c", line[i]);
			i += 1;
		}
		else {
			printf("\nNormal String Detected: ");
			printf("%c", line[i]);
			inline_state = NORMAL_STRING;
			i += 1;
		}
	}
	printf("\nEnd of Line.\n");
}
