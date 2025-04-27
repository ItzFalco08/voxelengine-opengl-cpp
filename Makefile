all:
	g++ ./test.cpp -o test
	./test
cleanup:
	rm test