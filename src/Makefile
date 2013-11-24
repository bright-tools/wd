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
	strip $(TGT)

depend:
	makedepend $(C_SRC)

# DO NOT DELETE

cmdln.o: cmdln.h /usr/include/sys/param.h /usr/include/sys/types.h
cmdln.o: /usr/include/_ansi.h /usr/include/newlib.h /usr/include/sys/config.h
cmdln.o: /usr/include/machine/ieeefp.h /usr/include/sys/features.h
cmdln.o: /usr/include/machine/_types.h /usr/include/machine/_default_types.h
cmdln.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
cmdln.o: /usr/include/machine/types.h /usr/include/limits.h
cmdln.o: /usr/include/features.h /usr/include/sys/cdefs.h
cmdln.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
cmdln.o: /usr/include/stdio.h /usr/include/sys/reent.h
cmdln.o: /usr/include/sys/stdio.h /usr/include/unistd.h
cmdln.o: /usr/include/sys/unistd.h /usr/include/stdlib.h
cmdln.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
cmdln.o: /usr/include/pwd.h /usr/include/string.h /usr/include/sys/string.h
dir_list.o: dir_list.h /usr/include/stdlib.h /usr/include/machine/ieeefp.h
dir_list.o: /usr/include/_ansi.h /usr/include/newlib.h
dir_list.o: /usr/include/sys/config.h /usr/include/sys/features.h
dir_list.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
dir_list.o: /usr/include/machine/_types.h
dir_list.o: /usr/include/machine/_default_types.h /usr/include/sys/lock.h
dir_list.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
dir_list.o: /usr/include/stdio.h /usr/include/sys/types.h
dir_list.o: /usr/include/machine/types.h /usr/include/sys/stdio.h
dir_list.o: /usr/include/sys/cdefs.h /usr/include/stdint.h
dir_list.o: /usr/include/bits/wordsize.h /usr/include/assert.h
dir_list.o: /usr/include/string.h /usr/include/sys/string.h
dir_list.o: /usr/include/errno.h /usr/include/sys/errno.h
dir_list.o: /usr/include/sys/param.h /usr/include/limits.h
dir_list.o: /usr/include/features.h /usr/include/sys/stat.h
dir_list.o: /usr/include/time.h /usr/include/machine/time.h
wd.o: cmdln.h /usr/include/sys/param.h /usr/include/sys/types.h
wd.o: /usr/include/_ansi.h /usr/include/newlib.h /usr/include/sys/config.h
wd.o: /usr/include/machine/ieeefp.h /usr/include/sys/features.h
wd.o: /usr/include/machine/_types.h /usr/include/machine/_default_types.h
wd.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
wd.o: /usr/include/machine/types.h /usr/include/limits.h
wd.o: /usr/include/features.h /usr/include/sys/cdefs.h /usr/include/stdint.h
wd.o: /usr/include/bits/wordsize.h dir_list.h /usr/include/stdlib.h
wd.o: /usr/include/sys/reent.h /usr/include/machine/stdlib.h
wd.o: /usr/include/alloca.h /usr/include/stdio.h /usr/include/sys/stdio.h
