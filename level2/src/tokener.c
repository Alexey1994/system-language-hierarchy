Number  line_number = 1;
Number  const_number_value;
Stack   asm_value;
Stack   name_value;
Stack   const_string_value;


typedef enum {
	EOF_TOKEN = 256,
	SHIFT_LEFT_TOKEN,
	SHIFT_RIGHT_TOKEN,
	EQUAL_TOKEN,
	NOT_EQUAL_TOKEN,
	LESSER_OR_EQUAL_TOKEN,
	GREATHER_OR_EQUAL_TOKEN,
	CONST_NUMBER_TOKEN,
	CONST_STRING_TOKEN,
	ARROW_TOKEN,
	NOT_ARROW_TOKEN,
	NAME_TOKEN,
	LINK_TOKEN,
	
	IF_TOKEN,
	ELSE_TOKEN,
	WHILE_TOKEN,
	FROM_TOKEN,
	TO_TOKEN,
	
	UNARY_MINUS_OPERATION,
	GET_ADDRESS_OPERATION,
	ASM_TOKEN,
	DECLARE_VARIABLE_TOKEN,
	GLOBAL_VARIABLE_TOKEN,
	LOCAL_VARIABLE_TOKEN,
	CALL_TOKEN,
	SYSTEM_CALL_TOKEN
}
Token;


Byte token_names[][16] = {
	"<EOF>",
	"<<",
	">>",
	"==",
	"!=",
	"<=",
	">=",
	"const number",
	"const string",
	"=>",
	"!=>",
	"name",
	"link",
	
	"if",
	"else",
	"while",
	"from",
	"to"
};


void print_token(Token token)
{
	print_error("прочитано ");

	if(token < 256) {
		print_error("%c", token);
	}
	else if(token == ASM_TOKEN) {
		print_error("`%s`", asm_value);
	}
	else if(token == CONST_NUMBER_TOKEN) {
		print_error("%d", const_number_value);
	}
	else {
		print_error("%s", token_names[token - 256]);
	}
	
	print_error("\n");
}


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
			case ' ': {
				next();
				goto repeat;
			}
			
			case '#': {
				do {
					next();
				}
				while(!end_of_stream && head() != '\n');

				if(end_of_stream) {
					return EOF_TOKEN;
				}

				++line_number;
				goto repeat;
			}
			
			case '`': {
				initialize_stack(&asm_value, 16);
				
				for(;;) {
					next();
					
					if(end_of_stream) {
						error(line_number, "отсутствует `");
					}
					
					head_character = head();
					
					if(head_character == '`') {
						next();
						break;
					}
					
					if(head_character == '\n') {
						++line_number;
					}

					add_bytes_in_stack(&asm_value, &head_character, 1);
				}
				
				head_character = '\0';
				add_bytes_in_stack(&asm_value, &head_character, 1);
				
				return ASM_TOKEN;
			}
			
			case '!': {
				next();
				
				switch(head()) {
					case '=':
						next();
						
						if(head() == '>') {
							next();
							return NOT_ARROW_TOKEN;
						}
						
						return NOT_EQUAL_TOKEN;
					
					default:
						return '!';
				}
				break;
			}
			
			case '=': {
				next();
				
				switch(head()) {
					case '=':
						next();
						return EQUAL_TOKEN;
					
					case '>':
						next();
						return ARROW_TOKEN;
					
					default:
						return '=';
				}
				break;
			}
			
			case '<': {
				next();
				
				switch(head()) {
					case '=':
						next();
						return LESSER_OR_EQUAL_TOKEN;
					
					case '<':
						next();
						return SHIFT_LEFT_TOKEN;
					
					case '>':
						next();
						return NOT_EQUAL_TOKEN;
					
					default:
						return '<';
				}
				break;
			}
			
			case '>': {
				next();
				
				switch(head()) {
					case '=':
						next();
						return GREATHER_OR_EQUAL_TOKEN;
					
					case '>':
						next();
						return SHIFT_RIGHT_TOKEN;
					
					default:
						return '>';
				}
				break;
			}
			
			case '0'...'9': {
				const_number_value = 0;
				do {
					const_number_value = const_number_value * 10 + (head_character - '0');
					next();
					head_character = head();
				}
				while(!end_of_stream && head_character >= '0' && head_character <= '9');

				return CONST_NUMBER_TOKEN;
			}
			
			case '\'': {
				next();
				
				if(end_of_stream) {
					error(line_number, "неожиданный конец в '");
				}
				
				const_number_value = 0;
				
				for(;;) {
					head_character = head();
					next();
				
					if(end_of_stream) {
						error(line_number, "неожиданный конец в '");
					}
					
					if(head_character == '\'') {
						next();
						break;
					}
					else if(head_character == '\\') {
						head_character = head();
						next();
						
						if(end_of_stream) {
							error(line_number, "неожиданный конец в \"");
						}
						
						switch(head_character) {
							case 'n': {
								head_character = '\n';
								break;
							}
							
							case 'r': {
								head_character = '\r';
								break;
							}
							
							case 't': {
								head_character = '\t';
								break;
							}
							
							case 'b': {
								head_character = '\b';
								break;
							}
						}
					}
					
					const_number_value = head_character;
				}
				
				return CONST_NUMBER_TOKEN;
			}
			
			case '"': {
				next();
				
				if(end_of_stream) {
					error(line_number, "неожиданный конец в \"");
				}
				
				initialize_stack(&const_string_value, 16);
				
				for(;;) {
					head_character = head();
					next();
				
					if(end_of_stream) {
						error(line_number, "неожиданный конец в \"");
					}
					
					if(head_character == '"') {
						next();
						break;
					}
					else if(head_character == '\\') {
						head_character = head();
						next();
						
						if(end_of_stream) {
							error(line_number, "неожиданный конец в \"");
						}
						
						switch(head_character) {
							case 'n': {
								head_character = '\n';
								break;
							}
							
							case 'r': {
								head_character = '\r';
								break;
							}
							
							case 't': {
								head_character = '\t';
								break;
							}
							
							case 'b': {
								head_character = '\b';
								break;
							}
						}
					}
					
					add_bytes_in_stack(&const_string_value, &head_character, 1);
				}
				
				head_character = '\0';
				add_bytes_in_stack(&const_string_value, &head_character, 1);
				
				return CONST_STRING_TOKEN;
			}
			
			case 'a'...'z':
			case 'A'...'Z':
			case '_': {
				initialize_stack(&name_value, 16);
				
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

				for(i = IF_TOKEN - 256; i <= TO_TOKEN - 256; ++i) {
					if(!compare_null_terminated_bytes(token_names[i], name_value.data)) {
						deinitialize_stack(&name_value);
						return i + 256;
					}
				}
				
				if(head() == ':') {
					next();
					return LINK_TOKEN;
				}

				return NAME_TOKEN;
			}

			default: {
				head_character = head();
				next();
				return head_character;
			}
		}
	}
}