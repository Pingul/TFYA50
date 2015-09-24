CCC = gcc
CPPFLAGS = -Wall -Wpedantic -I/opt/local/include/ -std=c++11
FRAMEWORKS = -lstdc++
DEPS = vector.h

main: vector.cc
	$(CCC) $(CPPFLAGS) $(FRAMEWORKS) -o main $^

%.o: %.cc $(DEPS)
	$(CCC) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f *.o main 