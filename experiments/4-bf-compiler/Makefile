all: bf output.ll output.s output run

bf: bf.c
	clang bf.c -o bf

output.ll: bf input.bf
	./bf < ./input.bf > ./output.ll

output.s: output.ll
	llc ./output.ll -o ./output.s

output: output.s
	clang ./output.s -o ./output

run:
	./output

clean:
	rm -f bf output

.PHONY: clean run
