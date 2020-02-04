
CC=cc
LIB_TARGET=libprocname.so
EXE_TARGET=procname

SRC=src/
ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

$(EXE_TARGET): $(SRC)procname.c
	$(CC) -Wall -g -o $(EXE_TARGET) $(SRC)procname.c

$(LIB_TARGET): $(SRC)libprocname.c
	$(CC) -Wall -Werror -fPIC -shared -o $(LIB_TARGET) $(SRC)libprocname.c

install: $(LIB_TARGET) $(EXE_TARGET)
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(LIB_TARGET) $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 777 $(EXE_TARGET) $(DESTDIR)$(PREFIX)/bin/





