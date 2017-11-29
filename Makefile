CXXFLAGS=-m32 -DKXVER=3
LDLIBS=-pthread
g:g.C c.o k.h
c.o:
	curl https://raw.githubusercontent.com/KxSystems/kdb/master/l32/c.o -o c.o
k.h:
	curl https://raw.githubusercontent.com/KxSystems/kdb/master/c/c/k.h -o k.h
.PHONY:clean
clean:
	$(RM) g c.o k.h
