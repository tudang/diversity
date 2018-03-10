#!/usr/bin/env python

import threading

def hello(*args, **kwargs):
    for x in args:
        print x

    for k,v in kwargs.items():
        print k, v

t = threading.Timer(1, hello, ['1', '2'], {'arg1': 1, 'arg2': 2})

t.start()
