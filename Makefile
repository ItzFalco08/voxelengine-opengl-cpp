all:
	./test
test: test.cpp
	g++ test.cpp -o test
cleanup:
	rm test