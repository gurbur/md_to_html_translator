#include "list_head.h"

// -------------------- Tokenizer Types --------------------

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
	IMAGE_LINK,
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

// -------------------- Parser Types --------------------

typedef enum {
	NODE_DOCUMENT = 0,
	NODE_HEADING1,
	NODE_HEADING2,
	NODE_HEADING3,
	NODE_CODE_BLOCK,
	NODE_LINE,
	NODE_ORDERED_LIST,
	NODE_UNORDERED_LIST,
	NODE_LIST_ITEM,
	NODE_PARAGRAPH,
	NODE_IMAGE_LINK,
	NODE_NORMAL_TEXT,
	NODE_ITALIC_TEXT,
	NODE_BOLD_TEXT,
	NODE_ITALIC_AND_BOLD_TEXT,
	NODE_INLINE_CODE,
	NODE_LINK,
} ast_node_types;

typedef struct {
	struct list_head sibling;
	struct list_head child;
	ast_node_types type;
	char* data1;
	char* data2;
} ast_node;

// -------------------- HTML Generator --------------------


