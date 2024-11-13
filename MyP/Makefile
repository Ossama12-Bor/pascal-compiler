all :	myp

syntax : lexic	myp.y
	bison -v -y  -d  myp.y
lexic : myp.l
	flex myp.l

PCode/PCode.o : PCode/PCode.c PCode/PCode.h
	cd PCode; make pcode

myp		:	syntax Table_des_symboles.c Table_des_chaines.c PCode/PCode.o
	gcc -o myp lex.yy.c y.tab.c PCode/PCode.o Table_des_symboles.c Table_des_chaines.c

clean		:	
	rm -f lex.yy.c *.o y.tab.h y.tab.c myp *~ y.output; cd PCode; make clean

