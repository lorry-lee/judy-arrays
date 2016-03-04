# -*- python -*-

import os

APPNAME = 'judy'
VERSION = '1.0'

top = '.'
out = 'build'

def configure(conf):
    conf.load('compiler_c')

def options(opt):
    opt.load('compiler_c')

def build(bld):
    flags = {
        'cflags'        : ['-O2', '-Wall', '-Wextra'],
        'dflags'        : ['-g'],
    }
    source = bld.path.ant_glob('src/*.c', excl=['**/test_*.c'])

    features = {
        'source'        : source,
        'target'        : 'sh-judy',
    }
    features.update(flags)
    bld.shlib(**features)

    features = {
        'source'        : source,
        'target'        : 'st-judy',
    }
    features.update(flags)
    bld.stlib(**features)

    for test in bld.path.ant_glob('src/test_*.c'):
        features = {
            'source'        : [test],
            'target'        : os.path.splitext(str(test))[0],
            'use'           : 'st-judy',
            'lib'         : ['cunit', 'crypto', 'dl'],
        }
        features.update(flags)
        bld.program(**features)
