#!/bin/sh

gcc -shared -fPIC -o poc.so poc.c -I lua