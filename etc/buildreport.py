#!/usr/bin/env python

import sys

if __name__ == '__main__':
    sys.stdout.write(open('etc/template.html').read() % {
        'marked': open('etc/marked.js').read(),
        'report': sys.stdin.read(),
        })
