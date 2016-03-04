# -*- python -*-

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
    features = {
        'source'        : bld.path.ant_glob('src/*.c'),
        'target'        : 'judy',
    }
    features.update(flags)
    bld.shlib(**features)
    bld.stlib(**features)
