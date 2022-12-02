#include <types.c>
#include <stream.c>
#include <data-structures/stack.c>

#include "link.c"
#include "tokener.c"
#include "parser.c"
#include "generator.c"


Number main()
{
	Node* instruction;
	Stack program;
	
	initialize_stack(&links, 512);
	initialize_stack(&program, 512);

	parse_program(&program);
	calculate_links(&program);
	generate(&program);
	
	return 0;
}