typedef struct {
	Byte*  name;
	Number address;
}
Link;


Stack links;


Link* find_link(Byte* name)
{
	Number i;
	Link*  link;
	
	for(i = 0; i < links.size; i += sizeof(Link*)) {
		link = *(Link**)(links.data + i);
		
		if(!compare_null_terminated_bytes(link->name, name)) {
			return link;
		}
	}
	
	return 0;
}


Link* create_link(Byte* name)
{
	if(find_link(name)) {
		return 0;
	}
	
	Link* link;
	
	link = allocate_memory(sizeof(Link));
	link->name = name;
	link->address = 0;
	
	add_bytes_in_stack(&links, &link, sizeof(link));
	
	return link;
}
