main: 
	gcc -Wall -g -pedantic business_logic.c generic_utils.c generic_data_structures/q-sort.c generic_data_structures/iterator.c generic_data_structures/linked_list.c generic_data_structures/hash_table.c

run:
	make main
	valgrind --leak-check=full ./a.out