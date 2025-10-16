#!/bin/sh

gcc -Wl,-E -llua5.2 -shared -fPIC -o poc.so poc.c -I lua