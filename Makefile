CC=gcc
CFLAGS=-s -Os
# fill in all your make rules

vm_x2017: vm_x2017.c
	$(CC) $(CFLAGS) $^ -o $@

objdump_x2017: objdump_x2017.c
	$(CC) $(CFLAGS) $^ -o $@


tests: 
	echo "tests"
run_tests: export ASAN_OPTIONS=verify_asan_link_order=0
run_tests:
	bash test.sh

clean:
	rm vm_x2017
	rm objdump_x2017


