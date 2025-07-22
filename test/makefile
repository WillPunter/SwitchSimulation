CC := gcc
INCLUDE := -I./../src/simulator -I./../src/data_structures

clean:
	# note: we use @echo, because just the normal echo command would show the
	# echo command.
	@echo Cleaning repository...
	rm -f demo ./data_structures/test_heap ./data_structures/test_hash_table

demo:
	@echo Building demo tests...
	$(CC) demo.c $(INCLUDE) -o demo

heap:
	@echo Building heap tests...
	$(CC) ./data_structures/test_heap.c ./../src/data_structures/heap.c $(INCLUDE) -o ./data_structures/test_heap

hash_table:
	@echo Building hash table tests...
	$(CC) ./data_structures/test_hash_table.c ./../src/data_structures/hash_table.c $(INCLUDE) -o ./data_structures/test_hash_table

build: demo heap hash_table

test: build
	@echo Running all tests...
	./demo
	./data_structures/test_heap
	./data_structures/test_hash_table

check: build
	@echo Running memory checks...
	valgrind ./demo
	valgrind ./data_structures/test_heap
	valgrind ./data_structures/test_hash_table