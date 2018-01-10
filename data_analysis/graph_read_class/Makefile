# Make file 

test: test.o graph.o node.o
	g++ -o $@ $< -std=c++0x

test.o: test.C graph.C node.C graph.h node.h
	g++ -c $< -std=c++0x

graph.o: graph.C graph.h node.C node.h
	g++ -c $< -std=c++0x

node.o: node.C node.h
	g++ -c $<

memcheck: test
	valgrind --leak-check=full ./test < g2.in

.PHONY: clean

clean: 
	rm test *.o
