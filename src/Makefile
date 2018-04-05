C_SRC := cmdln.c dir_list.c wd.c
ifeq ($(TARGET),win32)
  C_SRC += shrtcut.c  win32.c
  MINGW_CC= i686-pc-mingw32-gcc.exe
  MINGW_LDFLAGS = -L/usr/i686-pc-mingw32/sys-root/mingw/lib/ -lole32 -luuid
  LDFLAGS = $(MINGW_LDFLAGS)
  CC = $(MINGW_CC)
else
  C_SRC += posix.c
  ifeq ($(TARGET),)
    TARGET=$(shell uname -o)
  endif
endif
CDEFS   = -DTARGET=$(TARGET)
CFLAGS  = -O3 -g -Wall $(CDEFS)
OBJS    = $(C_SRC:.c=.o)
TGT     = wd

$(TGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

# Rule to install all the packages needed to develop this
#  under Cygwin
cyg_install:
	setup-x86.exe -q -P ctags,mingw-gcc-g++,make,splint,doxygen

clean:
	rm -rf $(OBJS) $(TGT)

tags:
	ctags --tag-relative --extra=f -R .

splint:
	# -preproc used because splint doesn't seem to like varadic macros
	splint $(C_SRC) $(CDEFS) -preproc +posix-lib +bool-int

release: $(TGT)
	strip -s $(TGT)
	ldd $(TGT)
	ls -la $(TGT)

depend:
	makedepend $(C_SRC)

docs:
	rm -rf html/search html/*.js html/*.css html/*.html html/*.png
	doxygen ../docs/Doxyfile

mingw:
	LDFLAGS="$(MINGW_LDFLAGS)" CC="$(MINGW_CC)" TARGET=win32 $(MAKE) clean release
 
.PHONY: package
package:
	rm -rf package
	mkdir package
	mkdir package/mingw
	mkdir package/cygwin
	CC=gcc $(MAKE) clean release
	cp wd.exe package/cygwin
	$(MAKE) mingw
	cp wd.exe package/mingw
	cp ../README.md package
	cp ../shell/wd.* ../shell/wcd.bat package

# DO NOT DELETE
