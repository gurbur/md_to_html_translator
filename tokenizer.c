#define MAX_LINE_LENGTH 2048
#define MAX_INLINE_BUFFER 100
#define DEFAULT_OUTPUT "output.tokens"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "types.h"

/*
 * TODO:
 * 0. [x] define data structures
 * 1. [x] load markdown files
 * 2. [x] send it to block tokenizer function line by line
 * 3. [x] check if given line is block element
 * 4. [x] if given block element has string, send it to inline tokenizer function
 * 5. [x] if given string has inline element, tokenize it
 * 6. [x] print the output into specific file format
*/

block_element_type block_state = NONE_BLOCK;

FILE* load_file(char* file_dir, char* mode);
void process_block(char* line, FILE* output);
void process_inline(char* line, FILE* output);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "[ERROR] Usage: %s <.md file> [-o <output file name>]\n", argv[0]);
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
		fclose(input);
		return EXIT_FAILURE;
	}

	FILE *output;
	output = load_file(output_dir, "w");
	
	if (output == NULL) {
		fclose(input);
		fclose(output);
		return EXIT_FAILURE;
	}

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), input)){
		process_block(line, output);
	}
	
	fclose(input);
	fclose(output);

	printf("[COMPLETE] End of Tokenizing: %s -> %s\n", input_dir, output_dir);
	return EXIT_SUCCESS;
}

// ******************************

FILE* load_file(char* file_dir, char* mode) {
	FILE *file = fopen(file_dir, mode);
	if (!file) {
		if (mode[0] == 'r') {
			perror("[ERROR] Can't locate file");
			return NULL;
		}
		else if (mode[0] == 'w') {
			perror("[ERROR] Can't create output file");
			return NULL;
		}
	}

	return file;
}

void process_block(char* line, FILE* output) {
	bool skip_flag = false;
	int num = -1, offset = 0;
	if (block_state == CODE_BLOCK) {
		if (strncmp(line, "```", 3) == 0) {
			//printf("End of Code Block\n");
			//fprintf(output, "\n");
			block_state = NONE_BLOCK;
			skip_flag = true;
		}
		else {
			//printf("Keep processing Code Block...\n");
			fprintf(output, "\t%s", line);
			skip_flag = true;
		}
	}
	else if (block_state == ORDERED_LIST) {
		if (sscanf(line, "%d.%n", &num, &offset) == 1 && line[offset] == ' ' && isalnum(line[offset + 1])) { // edge case: what if there is 2 or more digits? (ex. 13, 143, ...) -> handled on Mar.19, 2025
			//printf("Keep processing Ordered List(%c)...\n", line[0]);
			fprintf(output, "ORDERED_LIST\n\tNUM\n\t\t%c\n", line[0]);
			skip_flag = true;
			line += offset + 1;
		}
		else {
			//printf("End of Ordered List\n");
			//fprintf(output, "\n");
			block_state = NONE_BLOCK;
		}
	}
	else if (block_state == UNORDERED_LIST) {
		if (strncmp(line, "- ", 2) == 0) {
			//printf("Keep processing Unordered List...\n");
			fprintf(output, "UNORDERED_LIST\n");
			skip_flag = true;
			line = &line[2];
		}
		else {
			//printf("End of Unordered List\n");
			//fprintf(output, "\n");
			block_state = NONE_BLOCK;
		}
	}
	if (skip_flag) {
		// do_nothing();
	}
	else if (strncmp(line, "# ", 2) == 0) {
		//printf("Heading 1 detected\n");
		fprintf(output, "HEADING1\n");
		block_state = HEADING1;
		line = &line[2];
	}
	else if (strncmp(line, "## ", 3) == 0) {
		//printf("Heading 2 detected\n");
		fprintf(output, "HEADING2\n");
		block_state = HEADING2;
		line = &line[3];
	}
	else if (strncmp(line, "### ", 4) == 0) {
		//printf("Heading 3 detected\n");
		fprintf(output, "HEADING3\n");
		block_state = HEADING3;
		line = &line[4];
	}
	else if (strncmp(line, "```", 3) == 0) {
		//printf("Code Block detected\n");
		fprintf(output, "CODE_BLOCK\n");
		block_state = CODE_BLOCK;
		if (line[3] != '\n') {
			//printf("Code type is %s", &line[3]);
			fprintf(output, "\tCODE_TYPE\n\t\t%s", &line[3]);
		}
	}
	else if (strncmp(line, "***\n", 4) == 0 || strncmp(line, "---\n", 4) == 0) {
		//printf("Line detected\n");
		fprintf(output, "LINE\n");
		block_state = LINE;
	}
	else if (sscanf(line, "%d.%n", &num, &offset) == 1 && line[offset] == ' ' && isalnum(line[offset + 1])) {
		fprintf(output, "ORDERED_LIST\n\tNUM\n\t\t%c\n", line[0]);
		block_state = ORDERED_LIST;
		line += offset + 1;
		//printf("Ordered List detected\n");
	}
	else if (strncmp(line, "- ", 2) == 0) {
		//printf("Unordered List detected\n");
		fprintf(output, "UNORDERED_LIST\n");
		block_state = UNORDERED_LIST;
		line = &line[2];
	}
	else if (strncmp(line, "![[", 3) == 0) {
		fprintf(output, "IMAGE_LINK\n\t");
		block_state = IMAGE_LINK;
		line = &line[3];
		while(strncmp(line, "]]\n", 2) != 0) {
			fprintf(output, "%c", line[0]);
			line = &line[1];
		}
		block_state = NONE_BLOCK;
		fprintf(output, "\n");
	}
	else if (block_state == PARAGRAPH) {
		//printf("Keep processing Paragraph...\n");
		//fprintf(output, "[DEBUG] Paragraph: %s", line);
		if (line[0] == '\n') {
			block_state = NONE_BLOCK;
		}
	}
	else {
		//printf("Paragraph detected\n");
		fprintf(output, "PARAGRAPH\n");
		block_state = PARAGRAPH;
	}
	
	if (block_state == HEADING1 || block_state == HEADING2 ||\
		block_state == HEADING3 || block_state == ORDERED_LIST ||\
		block_state == UNORDERED_LIST || block_state == PARAGRAPH)
	{
		process_inline(line, output);
	}

}

void process_inline(char* line, FILE* output) {
	inline_element_type inline_state = NONE_INLINE;
	//printf("inline processing: %s", line);
	int i = 0;
	char buffer[MAX_INLINE_BUFFER];

	while(line[i] != '\0') {
		bool skip_flag = false;
		if (inline_state == ITALIC) {
			if (line[i] == '*') {
				//printf("\nEnd of Italic\n");
				//printf("\n");
				//fprintf(output, "\n");
				if (line[i + 1] != '\n') fprintf(output, "\n");
				inline_state = NONE_INLINE;
				i += 1;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == BOLD) {
			if (line[i] == '*' && line[i + 1] == '*') {
				//printf("\nEnd of Bold\n");
				//printf("\n");
				//fprintf(output, "\n");
				if (line[i + 2] != '\n') fprintf(output, "\n");
				inline_state = NONE_INLINE;
				i += 2;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == ITALIC_AND_BOLD) {
			if (line[i] == '*' && line[i + 1] == '*' && line[i + 2] == '*') {
				//printf("\nEnd of Italic and Bold\n");
				//printf("\n");
				//fprintf(output, "\n");
				if (line[i + 3] != '\n') fprintf(output, "\n");
				inline_state = NONE_INLINE;
				i += 3;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == INLINE_CODE) {
			if (line[i] == '`') {
				//printf("\nEnd of Inline Code\n");
				//printf("\n");
				fprintf(output, "\n");
				inline_state = NONE_INLINE;
				i += 1;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == LINK) {
			if (line[i] == ']' && line[i + 1] == ']') {
				//printf("\nEnd of Link\n");
				//printf("\n");
				fprintf(output, "\n");
				inline_state = NONE_INLINE;
				i += 2;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == LINK_SHOWN) {
			if (line[i] == ']' && line[i + 1] == '(') {
				//printf("\nEnd of Showing Link\nHidden Link: ");
				//printf("\nHidden Link: ");
				fprintf(output, "\n\t\tHIDDEN_LINK\n\t\t\t");
				inline_state = LINK_HIDDEN;
				i += 2;
				skip_flag = true;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}
		else if (inline_state == LINK_HIDDEN) {
			if (line[i] == ')') {
				//printf("\nEnd of Hidden Link\n");
				//printf("\n");
				fprintf(output, "\n");
				inline_state = NONE_INLINE;
				i += 1;
			}
			else {
				//printf("%c", line[i]);
				fprintf(output, "%c", line[i]);
				i += 1;
				skip_flag = true;
			}
		}

		if (skip_flag) {
			//do_nothing();
		}
		else if (line[i] == '\n') {
			i += 1;
		}
		else if (line[i] == '\0') {
			break;
		}
		else if (line[i] == '*' && line[i + 1] == '*' && line[i + 2] == '*') {
			//printf("	Italic and Bold Detected: ");
			if (inline_state == NORMAL_STRING) fprintf(output, "\n");
			fprintf(output, "\tITALIC_AND_BOLD\n\t\t");
			inline_state = ITALIC_AND_BOLD;
			i += 3; // skip ***
		}
		else if (line[i] == '*' && line[i + 1] == '*') {
			//printf("	Bold Detected: ");
			if (inline_state == NORMAL_STRING) fprintf(output, "\n");
			fprintf(output, "\tBOLD\n\t\t");
			inline_state = BOLD;
			i += 2;
		}
		else if (line[i] == '*') {
			//printf("	Italic Detected: ");
			if (inline_state == NORMAL_STRING) fprintf(output, "\n");
			fprintf(output, "\tITALIC\n\t\t");
			inline_state = ITALIC;
			i += 1;
		}
		else if (line[i] == '`') {
			//printf("\n	Inline Code Detected: ");
			if (inline_state == NORMAL_STRING) fprintf(output, "\n");
			fprintf(output, "\tINLINE_CODE\n\t\t");
			inline_state = INLINE_CODE;
			i += 1;
		}
		else if (line[i] == '[' && line[i + 1] == '[') {
			//printf("\n	Link Detected: ");
			if (inline_state == NORMAL_STRING) fprintf(output, "\n");
			fprintf(output, "\tLINK\n\t\t");
			inline_state = LINK;
			i += 2;
		}
		else if (line[i] == '[') {
			//printf("\n	Hidden/Shown Link Detected.\n");
			if (inline_state == NORMAL_STRING) fprintf(output, "\n");
			fprintf(output, "\tHIDDEN/SHOWN_LINK\n\t\tLINK_SHOWN\n\t\t\t");
			inline_state = LINK_SHOWN;
			i += 1;
		}
		else if (line[i] == '\\') {
			//printf("\n	Escape Character Detected: %c", line[i + 1]);
			fprintf(output, "%c", line[i + 1]);
			inline_state = NORMAL_STRING;
			i += 2;
		}
		else if (inline_state == NORMAL_STRING) {
			//printf("%c", line[i]);
			fprintf(output, "%c", line[i]);
			i += 1;
		}
		else {
			//printf("	Normal String Detected: ");
			//printf("%c", line[i]);
			fprintf(output,"\tNORMAL_STRING\n\t\t%c", line[i]);
			inline_state = NORMAL_STRING;
			i += 1;
		}
	}
	//printf("End of Line.\n");
	//printf("\n");
	if (inline_state != ITALIC && inline_state != BOLD && inline_state != ITALIC_AND_BOLD) {
		fprintf(output, "\n");
	}
}
