CCC = gcc
CPPFLAGS = -Wall -Wpedantic -I/opt/local/include/ -std=c++11
FRAMEWORKS = -lstdc++
DEPS = vector.h atom.h simulation.h mdbox.h lattice.h material.h

main: main.cc vector.cc atom.cc simulation.cc mdbox.cc lattice.cc material.cc
	$(CCC) $(CPPFLAGS) $(FRAMEWORKS) -o main $^

%.o: %.cc $(DEPS)
	$(CCC) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -f *.o main 