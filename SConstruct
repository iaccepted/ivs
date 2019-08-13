src = Glob('*.c')

paths = ['./lib/']
paths += ['./lib/vhost']

libs = ['pthread']

env = Environment()
env.Append(CPPPATH=paths)
env.Append(CCCOMSTR='CC $SOURCES') #define print format
env.Append(LINKCOMSTR='LINK $TARGET')
env.Append(LIBS=libs)

objs = SConscript('./lib/SConscript', duplicate=0)
objs += SConscript('./lib/vhost/SConscript', duplicate=0)

objs += env.Object(src)

env.Program(target = 'ivs-vswitchd', source=objs)
