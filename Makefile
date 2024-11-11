
txtest: tx.c
	gcc -O3 -DARRSIZE=4096 -DITERATIONS=1000000 tx.c -o $@

txbench: tx.c
	gcc -O3 -DARRSIZE=4096 -DITERATIONS=1000000 -DNOVALIDATE tx.c -o $@

clean: 
	rm txtest txbench
