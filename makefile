
$(info Starting to make assembler run file)
assembler: dataStructure.o dataValidation.o signs.o utils.o first_Iteration.o second_iteration.o assembler.o main.o
	gcc -g -lm -Wall dataStructure.o dataValidation.o signs.o utils.o first_Iteration.o second_iteration.o assembler.o main.o -pedantic -o assembler

$(info Starting to make dataStructure.o)
dataStructure.o: dataStructure.c
	gcc -c -ansi -Wall -pedantic dataStructure.c -o dataStructure.o

$(info Starting to make dataValidation.o)
dataValidation.o: dataValidation.c dataStructure.h
	gcc -c -ansi -Wall -pedantic dataValidation.c -o dataValidation.o

$(info Starting to make signs.o)
signs.o: dataStructure.h dataValidation.h signs.c
	gcc -c -g -ansi -Wall -pedantic signs.c -o signs.o 

$(info Starting to make utils.o)
utils.o: dataStructure.h signs.h dataValidation.h utils.c
	gcc -c -g -ansi -Wall -pedantic utils.c -o utils.o 

$(info Starting to make first_Iteration.o)
first_Iteration.o: utils.h dataStructure.h dataValidation.h first_Iteration.c
	gcc -c -ansi  -Wall -pedantic first_Iteration.c  -o first_Iteration.o

$(info Starting to make second_iteration.o)
second_iteration.o: utils.h dataStructure.h dataValidation.h second_iteration.c
		gcc -c -ansi -Wall -pedantic second_iteration.c  -o second_iteration.o

$(info Starting to make assembler.o)
assembler.o: dataStructure.h utils.h dataValidation.h first_Iteration.h second_iteration.h assembler.c
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

$(info Starting to make main.o)
main.o: assembler.h utils.h main.c
	gcc -c -ansi -Wall -pedantic main.c -o main.o

clear:
	rm -f *.o assemble