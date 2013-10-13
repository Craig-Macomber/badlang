build: full.out

l = clang++ `llvm-config --cppflags --ldflags --libs core jit native` -Wno-c++11-extensions -O0 -g
c = clang++ -Wno-c++11-extensions -O0 -g

runtime.o basicTypes.o: runtime/*.cpp runtime/*.h
	$c -c runtime/*.cpp

full.out: runtime.o basicTypes.o *.cpp *.h 
	$l *.cpp *.o -o full.out

run: full.out
	./full.out < test.bad

clean:
	rm *.out
	rm *.o