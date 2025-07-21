CC := gcc
INCLUDE := -I./../src/simulator

clean:
	# note: we use @echo, because just the normal echo command would show the
	# echo command.
	@echo Cleaning repository...
	rm -f demo ./simulator/test_heap

demo:
	@echo Building demo tests...
	$(CC) demo.c $(INCLUDE) -o demo

heap:
	@echo Building heap tests...
	$(CC) ./simulator/test_heap.c ./../src/simulator/heap.c $(INCLUDE) -o ./simulator/test_heap

hash_table:
	@echo Building hash table tests...
	$(CC) ./simulator/test_hash_table.c ./../src/simulator/hash_table.c $(INCLUDE) -o ./simulator/test_hash_table

build: demo heap hash_table

test: build
	@echo Running all tests...
	./demo
	./simulator/test_heap
	./simulator/test_hash_table

check: build
	@echo Running memory checks...
	valgrind ./demo
	valgrind ./simulator/test_heap
	valgrind ./simulator/test_hash_table