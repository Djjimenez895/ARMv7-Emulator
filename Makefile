PROGS = armemu sum_array_a test_a find_max_a fib_iter_a_v2 fib_rec_a find_str_a

OBJS = sum_array_a.o test_a.o find_max_a.o fib_iter_a_v2.o fib_rec_a.o find_str_a.o

CFLAGS = -g

all : armemu

sum_array_a : sum_array_a.s
	as -o sum_array_a.o sum_array_a.s

find_max_a : find_max_a.s
	as -o find_max_a.o find_max_a.s

fib_iter_a_v2 : fib_iter_a_v2
	as -o fib_iter_a_v2.o fib_iter_a_v2.s

fib_rec_a : fib_rec_a
	as -o fib_rec_a.o fib_rec_a.s

find_str_a : find_str_a
	as -o find_str_a.o find_str_a.s

test_a : test_a.s
	as -o test_a.o test_a.s

armemu: armemu.o sum_array_a.o test_a.o find_max_a.o fib_iter_a_v2.o fib_rec_a.o find_str_a.o
	gcc -g -o armemu armemu.o sum_array_a.o test_a.o find_max_a.o fib_iter_a_v2.o fib_rec_a.o find_str_a.o

clean:
	rm -rf ${PROGS} ${OBJS}
