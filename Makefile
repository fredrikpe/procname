LIB_TARGET=libprocname.so
EXE_TARGET=procname

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

$(EXE_TARGET): src/procname.c
	cc -Wall -o $(EXE_TARGET) src/procname.c

$(LIB_TARGET): src/libprocname.c
	cc -Wall -Werror -fPIC -shared -o $(LIB_TARGET) src/libprocname.c

install: $(LIB_TARGET) $(EXE_TARGET)
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(LIB_TARGET) $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 777 $(EXE_TARGET) $(DESTDIR)$(PREFIX)/bin/
	rm $(EXE_TARGET)
	rm $(LIB_TARGET)





