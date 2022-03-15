# jbmp library :: makefile
#
# https://github.com/johngineer/mword
#
# build instructions:
#
#   1. "make clean" (only required for rebuild)
#   2. "make" or "make libmword"
#
# installation:
#
#   1. "sudo make install"
#   2. "make demo"
#
# if the demo program compiles, then the library has been correctly
# installed.

libname := jbmp

obj := ./obj/

src := ./src/

mesg := ./gccmesg/

ofiles  := jbmp.o

diag := -fdiagnostics-color=always -fmessage-length=80

msgext := gccmesg.ansi

opts := -c -g -std=c99 

# builds the library archive from the object files
libjbmp: $(ofiles)
				ar rcs lib$(libname).a $(obj)*.o

pathcheck: $(obj) $(mesg)
				rmdir $(obj)
				rmdir $(mesg)
				mkdir $(obj)
				mkdir $(mesg)

# functions from mword_err.h
jbmp.o: $(src)jbmp.c $(src)jbmp.h $(src)jbmp_types.h
				gcc $(opts) $(diag) -o $(obj)jbmp.o $(src)jbmp.c 2> $(mesg)jbmp.$(msgext)

# deletes all the object files and forces full recompile
clean:
				rm -rf $(obj)*
				rm -rf $(mesg)*
				rm -rf *.bmp
				rm -rf *.a
				
# PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

# copies the library files to the linker paths set in the environment variables
install: libjbmp.a
				install -d $(DESTDIR)$(PREFIX)/lib/
				install -m 644 lib$(libname).a $(DESTDIR)$(PREFIX)/lib/
				install -d $(DESTDIR)$(PREFIX)/include/$(libname)/
				install -m 644 $(src)*.h $(DESTDIR)$(PREFIX)/include/$(libname)/

# the mword demo program (mword library must be installed)
demo: demo.c /usr/local/lib/libjbmp.a
				gcc -I. $(diag) -o demo demo.c -ljbmp 2> $(mesg)demo.$(msgext)

# the mword demo program (uses the local copy of libmword.a)
#demolocal: demo.c libmword.a
#				gcc -I. $(diag) -o demo demo.c -L. -lmword 2> $(mesg)demo.$(msgext)
