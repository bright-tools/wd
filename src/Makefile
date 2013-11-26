C_SRC := cmdln.c dir_list.c wd.c
CFLAGS  = -O3 -g -Wall
OBJS    = $(C_SRC:.c=.o)
TGT     = wd
CC      = i686-pc-mingw32-gcc.exe

$(TGT): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(OBJS) $(TGT)

release: $(TGT)
	strip -s $(TGT)
	ldd $(TGT)
	ls -la $(TGT)

depend:
	makedepend $(C_SRC)

docs:
	rm -rf html/search html/*.js html/*.css html/*.html html/*.png
	doxygen ../docs/Doxyfile

# DO NOT DELETE
