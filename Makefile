all: txtest txbench txtest_double txbench_double

txtest: tx.c
	gcc -O3 -DITERATIONS=1000000 tx.c -o $@
txtest_double: tx.c
	gcc -O3 -DDOUBLEBUFFER -DITERATIONS=1000000 tx.c -o $@

txbench: tx.c
	gcc -O3 -DITERATIONS=1000000 -DNOVALIDATE tx.c -o $@
txbench_double: tx.c
	gcc -O3 -DITERATIONS=1000000 -DNOVALIDATE -DDOUBLEBUFFER tx.c -o $@

clean: 
	rm txtest txbench txtest_double txbench_double
