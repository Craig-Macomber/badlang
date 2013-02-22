build: a.out

a.out: *.cpp *.h
	clang++ -g *.cpp `llvm-config --cppflags --ldflags --libs core jit native` -O0 -g -o a.out

run: a.out
	./a.out < test.bad

clean:
	rm a.out