nombre_tp = tp2

sources = analog.c main.c abb.c heap.c pila.c strutil.c hash.c hashing.c lista.c
headers = analog.h abb.h heap.h pila.h strutil.h hash.h hashing.h lista.h
extras = Makefile deps.mk
 
GCC_FLAGS = -g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion -o
VAL_FLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

git_add:
	git add $(sources) $(headers)
git_commit: git_add
	@read -p "Mensaje del commit: " MENSAJE; \
    git commit -m "$$MENSAJE"
	
git_pull:
	git pull origin
	
git_push:
	git push origin master
	
build: $(sources)
	gcc $(GCC_FLAGS) pruebas $(sources)
	 
run: build
	ulimit -v 100
	cat pruebas.txt | ./pruebas 200
	
chk: build
	cat pruebas.txt | valgrind $(VAL_FLAGS) ./pruebas 200
	
.PHONY : clean	
clean:
	-rm analog

zip:
	zip $(nombre_tp).zip $(sources) $(headers) $(extras) 
