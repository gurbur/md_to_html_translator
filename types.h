


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

