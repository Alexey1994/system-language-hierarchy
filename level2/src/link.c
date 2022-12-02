typedef struct {
	Byte* name;
}
Link;


Link* find_link(Byte* name)
{
	Number i;
	Link*  link;

	for(i = 0; i < current_function->links.size; i += sizeof(Link*)) {
		link = *(Link**)(current_function->links.data + i);

		if(!compare_null_terminated_bytes(link->name, name)) {
			return link;
		}
	}

	return 0;
}


Link* find_or_create_link(Byte* name)
{
	Link* link;

	link = find_link(name);

	if(!link) {
		link = allocate_memory(sizeof(Link));
		link->name = name;
		add_bytes_in_stack(&current_function->links, &link, sizeof(link));
	}

	return link;
}