src = Glob('*.c')

libs = ['pthread']

cflags = [
    '-Werror',
]

env = Environment()
env.Append(CCCOMSTR='CC $SOURCES')
env.Append(LINKCOMSTR='LINK $TARGET')
env.Append(LIBS=libs)
env.Append(CCFLAGS=cflags)

objs = SConscript('./lib/utils/SConscript', duplicate=0)
objs += SConscript('./lib/vhost/SConscript', duplicate=0)
objs += SConscript('./vswitch/SConscript', duplicate=0)

objs += env.Object(src)

env.Program(target='ivs-vswitchd', source=objs)
