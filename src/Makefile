C_SRC := cmdln.c dir_list.c wd.c
ifeq ($(TARGET),win32)
C_SRC += shrtcut.c
endif
CFLAGS  = -O3 -g -Wall 
MINGW_LDFLAGS = -L/usr/i686-pc-mingw32/sys-root/mingw/lib/ -lole32 -luuid
OBJS    = $(C_SRC:.c=.o)
TGT     = wd
MINGW_CC= i686-pc-mingw32-gcc.exe
#LDFLAGS = $(MINGW_LDFLAGS)
#CC = $(MINGW_CC)

$(TGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

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
 
.PHONY: package
package:
	rm -rf package
	mkdir package
	mkdir package/mingw
	mkdir package/cygwin
	CC=gcc $(MAKE) clean release
	cp wd.exe package/cygwin
	LDFLAGS=$(MINGW_LDFLAGS) CC=$(MINGW_CC) TARGET=win32 $(MAKE) clean release
	cp wd.exe package/mingw
	cp ../README.md package
	cp ../shell/wd.* ../shell/wcd.bat package

# DO NOT DELETE
