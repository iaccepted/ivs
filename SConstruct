src = Glob('*.c')

libs = ['pthread']
paths = ['./lib']

cflags = [
    '-Wall',
    '-Wl,--as-needed',
    '-Werror',
    '-O2',
    '-g',
]

Export('cflags')
env = Environment()
env.Append(CCCOMSTR='CC $SOURCES')
env.Append(LINKCOMSTR='LINK $TARGET')
env.Append(LIBS=libs)
env.Append(CPPPATH=paths)
env.Append(CCFLAGS=cflags)

objs = SConscript('./lib/memory/SConscript', dupicate=0)
objs += SConscript('./lib/epoll/SConscript', duplicate=0)
objs += SConscript('./lib/vhost/SConscript', duplicate=0)
objs += SConscript('./app/vswitch/SConscript', duplicate=0)
objs += SConscript('./lib/log/SConscript', duplicate=0)
objs += SConscript('./lib/dynamic_string/SConscript', duplicate=0)
objs += SConscript('./lib/netdev/SConscript', duplicate=0)

SConscript('./tests/SConscript', duplicate=0)

objs += env.Object(src)

env.Program(target='ivs-vswitchd', source=objs)
