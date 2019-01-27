all:
	cc -march=native -O3 -std=c11  -o timing_demo our_rand.c timing_demo.c  && ./timing_demo
