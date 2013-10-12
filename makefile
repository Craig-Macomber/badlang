build: a.out

l = clang++ `llvm-config --cppflags --ldflags --libs core jit native` -Wno-c++11-extensions -O0 -g
c = clang++ -Wno-c++11-extensions -O0 -g

runtime.o: runtime/*.cpp runtime/*.h
	$c -c runtime/*.cpp -o runtime.o

a.out: *.cpp *.h
	$l *.cpp -o a.out

full: runtime.o *.cpp *.h 
	$l *.cpp *.o -o full.out

run: a.out
	./a.out < test.bad

clean:
	rm a.out
	rm runtime.o