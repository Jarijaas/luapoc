#!/bin/sh


#LIBTOOL="libtool --tag=CC"
#$LIBTOOL --mode=compile cc -c poc.c -I/usr/include/lua5.2
#$LIBTOOL --mode=link cc -rpath /usr/lib/x86_64-linux-gnu/ -o libmodule.la module.lo
#mv .libs/libmodule.so.0.0.0 poc.so

gcc -O2 -fpic -I/usr/include/lua5.2 -c -o poc.o poc.c
gcc -O -shared -fpic -o poc.so poc.o

#gcc -Wl,-E -llua5.2 -shared -fPIC -o poc.so poc.c -I luaex