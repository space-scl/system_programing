
.PHONY: copy clean

copy:
	gcc main.c copyAtoB.c

clean:
	-rm -rf a.out
	-git clean -fdfdX

