# This makefile is on purpose not made with configure, to show how to use the library
# The make file requires that the fann library is installed (see ../README)

GCC=gcc

TARGETS = xor_train xor_test xor_test_fixed simple_train steepness_train simple_test robot mushroom cascade_train scaling_test scaling_train
DEBUG_TARGETS = xor_train_debug xor_test_debug xor_test_fixed_debug cascade_train_debug

all: $(TARGETS)

%: %.c Makefile
	$(GCC) -O3 $< -o $@ -lm -lfann

%_fixed: %.c Makefile
	$(GCC) -O3 -DFIXEDFANN $< -o $@ -lm -lfixedfann

clean:
	rm -f $(TARGETS) $(DEBUG_TARGETS) xor_fixed.data *.net *~ *.obj *.exe *.tds noscale.txt withscale.txt scale_test_results.txt

runtest: $(TARGETS)
	@echo
	@echo Training network
	./xor_train

	@echo
	@echo Testing network with floats
	./xor_test

	@echo
	@echo Testing network with fixed points
	./xor_test_fixed

#below this line is only for debugging the fann library

rundebugtest: $(DEBUG_TARGETS)
	@echo
	@echo Training network
	./xor_train_debug

	@echo
	@echo Testing network with floats
	./xor_test_debug

	@echo
	@echo Testing network with fixed points
	./xor_test_fixed_debug

runcompliancetest: $(DEBUG_TARGETS)
	./xor_train_debug

	@echo
	@echo Testing network with floats
	@cp xor_float.net xor_float_new.net
	@cp xor_float_backup.net xor_float.net
	@echo RUN
	@./xor_test_debug | grep XOR
	@echo CAT
	@cat xor_float_backup.out

#used to compare result when scale used and not used
runscaletest: scaling_train scaling_test
	echo Without scaling > noscale.txt
	./scaling_train >> noscale.txt
	./scaling_test >> noscale.txt
	echo With scaling > withscale.txt
	./scaling_train scale >> withscale.txt
	./scaling_test scale >> withscale.txt
	pr -m -t -T -w 150 noscale.txt withscale.txt > scale_test_results.txt
	@echo See scale_test_results.txt for results of scaling test

#compiletest is used to test whether the library will compile easily in other compilers
compiletest:
	gcc -O3 -ggdb -DDEBUG -Wall -Wformat-security -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -pedantic -ansi -I../src/ -I../src/include/ ../src/floatfann.c xor_train.c -o xor_train -lm 
	gcc -O3 -ggdb -DDEBUG -Wall -Wformat-security -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -pedantic -ansi -DFIXEDFANN -I../src/ -I../src/include/ ../src/fixedfann.c xor_test.c -o xor_test -lm 
	g++ -O3 -ggdb -DDEBUG -Wall -Wformat-security -Wfloat-equal -Wpointer-arith -Wcast-qual -Wsign-compare -pedantic -ansi -I../src/ -I../src/include/ ../src/floatfann.c xor_train.c -o xor_train -lm 

quickcompiletest:
	gcc -O -ggdb -DDEBUG -Wall -Wformat-security -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -pedantic -ansi -I../src/ -I../src/include/ ../src/floatfann.c ../examples/xor_train.c -o ../examples/xor_train -lm 

debug: $(DEBUG_TARGETS)

%_debug: %.c Makefile ../src/*c ../src/include/*h
	$(GCC) -ggdb -DDEBUG -Wall -ansi -I../src/ -I../src/include/ ../src/floatfann.c $< -o $@ -lm 

%_fixed_debug: %.c Makefile ../src/*c ../src/include/*h
	$(GCC) -ggdb -DDEBUG -Wall -ansi -DFIXEDFANN -I../src/ -I../src/include/ ../src/fixedfann.c $< -o $@ -lm

rundebug: $(DEBUG_TARGETS)
	@echo
	@echo Training network
	valgrind --leak-check=yes --show-reachable=yes --leak-resolution=high --db-attach=yes ./xor_train_debug

	@echo
	@echo Testing network with floats
	valgrind --leak-check=yes --show-reachable=yes --leak-resolution=high --db-attach=yes ./xor_test_debug

	@echo
	@echo Testing network with fixed points
	valgrind --leak-check=yes --show-reachable=yes --leak-resolution=high --db-attach=yes ./xor_test_fixed_debug
