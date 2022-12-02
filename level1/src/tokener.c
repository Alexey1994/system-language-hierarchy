#include <memory.c>


Number  line_number = 1;
Number  const_number_value;
Stack   name_value;


void error(Number line_number, Byte* parameters, ...)
{
	Utf8_To_Unicode_Converter converter;

	initialize_Utf8_To_Unicode_Converter(&converter, get_default_error_writer(), &write_character_in_console);
	write_Number(&converter, &write_byte_in_utf8_converter, line_number);
	write_null_terminated_bytes(&converter, &write_byte_in_utf8_converter, ": ");
	print_in_source(&converter, &write_byte_in_utf8_converter, parameters, &parameters + 1);
	write_null_terminated_bytes(&converter, &write_byte_in_utf8_converter, "\n");

	ExitProcess(1);
}


typedef enum {
	UNDEFINED_TOKEN,
	EOF_TOKEN,
	
	NUMBER_TOKEN,
	LINK_TOKEN,
	NAME_TOKEN,
	
	SET_DATA_ADDRESS_TOKEN,

	PUSH_TOKEN,
	POP_TOKEN,
	
	LOAD_TOKEN,
	STORE_TOKEN,
	LOCAL_TO_GLOBAL_TOKEN,

	SYSTEM_CALL_TOKEN,
	BEGIN_CALL_TOKEN,
	CALL_TOKEN,
	RETURN_TOKEN,
	RETURN_LINK_TOKEN,
	
	JUMP_TOKEN,
	JUMP_IF_ZERO_TOKEN,
	JUMP_IF_NOT_ZERO_TOKEN,
	
	NOT_TOKEN,
	
	SHIFT_LEFT_TOKEN,
	SHIFT_RIGHT_TOKEN,
	OR_TOKEN,
	AND_TOKEN,
	
	NEG_TOKEN,

	ADD_TOKEN,
	SUB_TOKEN,
	MUL_TOKEN,
	DIV_TOKEN,
	
	EQUAL_TOKEN,
	NOT_EQUAL_TOKEN,
	LESSER_TOKEN,
	LESSER_OR_EQUAL_TOKEN,
	GREATER_TOKEN,
	GREATER_OR_EQUAL_TOKEN,
	
	
	PUSH_NUMBER_TOKEN,
	PUSH_LINK_TOKEN
}
Token;


Byte token_names[][32] = {
	"undefined",
	"<EOF>",
	
	"number",
	"link",
	"name",
	
	"set_data_address",

	"push",
	"pop",
	
	"load",
	"store",
	"local_to_global",
	
	"system_call",
	"begin_call",
	"call",
	"return",
	"return_link",

	"jump",
	"jump_if_zero",
	"jump_if_not_zero",
	
	"not",
	
	"shift_left",
	"shift_right",
	"or",
	"and",
	
	"negate",

	"add",
	"sub",
	"multiply",
	"divide",
	
	"equal",
	"not_equal",
	"lesser",
	"lesser_or_equal",
	"greater",
	"greater_or_equal"
};


void print_token(Token token)
{
	print_error("прочитано %s", token_names[token]);
	
	if(token == NAME_TOKEN) {
		print_error("(%s)", name_value);
	}
	
	print_error("\n");
}


Token read_next_token()
{
	Byte   head_character;
	Number i;

	repeat: {
		head_character = head();
		
		if(end_of_stream) {
			return EOF_TOKEN;
		}

		switch(head_character) {
			case '\n':
				++line_number;
			case '\r':
			case '\t':
			case ' ':
				next();
				goto repeat;
			
			case '#': {
				do {
					next();
				}
				while(head() && head() != '\n');

				if(!head()) {
					return EOF_TOKEN;
				}

				++line_number;
				goto repeat;
			}
			
			case 'a'...'z':
			case 'A'...'Z':
			case '_': {
				initialize_stack(&name_value, 32);

				do {
					add_bytes_in_stack(&name_value, &head_character, 1);
					next();
					head_character = head();
				}
				while(!end_of_stream && (
					head_character >= 'a' && head_character <= 'z'
					|| head_character >= 'A' && head_character <= 'Z'
					|| head_character >= '0' && head_character <= '9'
					|| head_character == '_')
				);
				head_character = '\0';
				add_bytes_in_stack(&name_value, &head_character, 1);

				for(i = SET_DATA_ADDRESS_TOKEN; i <= GREATER_OR_EQUAL_TOKEN; ++i) {
					if(!compare_null_terminated_bytes(token_names[i], name_value.data)) {
						deinitialize_stack(&name_value);
						return i;
					}
				}
				
				if(head() == ':') {
					next();
					return LINK_TOKEN;
				}

				return NAME_TOKEN;
			}

			case '0'...'9':
			case '-': {
				Boolean is_minus = head() == '-';
				
				if(is_minus) {
					next();
					
					if(head() < '0' || head() > '9') {
						error(line_number, "ожидалось число");
					}
				}
				
				const_number_value = 0;
				do {
					const_number_value = const_number_value * 10 + (head() - '0');
					next();
				}
				while(!end_of_stream && head() >= '0' && head() <= '9');
				
				if(is_minus) {
					const_number_value = -const_number_value;
				}

				return NUMBER_TOKEN;
			}
			
			default: {
				return UNDEFINED_TOKEN;
			}
		}
	}
}