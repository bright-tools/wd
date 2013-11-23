C_SRC = cmdln.c dir_list.c wd.c
CFLAGS = -O3 -g -Wall
OBJS  = $(C_SRC:.c=.o)
TGT = wd

$(TGT): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(OBJS) $(TGT)

release:
	strip $(TGT)

depend:
	makedepend $(C_SRC)

# DO NOT DELETE

cmdln.o: cmdln.h /usr/include/stdio.h /usr/include/_ansi.h
cmdln.o: /usr/include/newlib.h /usr/include/sys/config.h
cmdln.o: /usr/include/machine/ieeefp.h /usr/include/sys/features.h
cmdln.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
cmdln.o: /usr/include/machine/_types.h /usr/include/machine/_default_types.h
cmdln.o: /usr/include/sys/lock.h /usr/include/sys/types.h
cmdln.o: /usr/include/machine/types.h /usr/include/sys/stdio.h
cmdln.o: /usr/include/sys/cdefs.h /usr/include/stdint.h
cmdln.o: /usr/include/bits/wordsize.h /usr/include/stdlib.h
cmdln.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
cmdln.o: /usr/include/unistd.h /usr/include/sys/unistd.h /usr/include/pwd.h
dir_list.o: dir_list.h
wd.o: cmdln.h dir_list.h /usr/include/stdlib.h /usr/include/machine/ieeefp.h
wd.o: /usr/include/_ansi.h /usr/include/newlib.h /usr/include/sys/config.h
wd.o: /usr/include/sys/features.h /usr/include/sys/reent.h
wd.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
wd.o: /usr/include/machine/_default_types.h /usr/include/sys/lock.h
wd.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
