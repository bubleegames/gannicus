import distutils.sysconfig

##SOURCES
engine_src = File(filter(lambda s: not("wrap" in s),
                         Glob("c++/engine/*.cc", strings=True)))
char_src = Glob("c++/characters/*.cc")
backend_src = engine_src + char_src
swig_src = Glob("c++/engine/*.i")
bmpout_src = [File("c++/bmp.cc")]
replay_src = [File("c++/replay.cc")]
gufg_exec_src = [File("c++/game.cc")]

##ENVIRONMENT
env = Environment(PYEXT_USE_DISTUTILS=True,
                  SWIGFLAGS=['-c++','-python'],
                  CPPPATH=[distutils.sysconfig.get_python_inc()],
                  CXXFLAGS="-O2 -g\
                            -Wno-overloaded-virtual\
                            -Wno-format\
                            -Wno-format-contains-nul\
                            -Wctor-dtor-privacy\
                            -std=c++11",
                  SHLIBPREFIX="_",
                  LIBS=['SDL','SDLmain','SDL_image','SDL_mixer','GL'])
miscdirs = [".config", ".data"]
for d in miscdirs:
  Execute(Mkdir(d))
wrappy = "c++/engine/engine.py"

#TARGETS
runtime = env.SharedLibrary('engine', backend_src + swig_src,)
bmpout = env.Program('bmpout', backend_src + bmpout_src)
replay = env.Program('replay', backend_src + replay_src)
gufg_exec = env.Program('gufg',backend_src + gufg_exec_src)


#INSTALL
env.Install(source=[wrappy,
                    runtime,
                    Glob("../src/python/*.py")],
            target="runtime")

env.Install(source=["runtime",
                    "../content",
                    "../src/scripts",
                    Glob("../info/*"),
                    bmpout,
                    replay,
                    gufg_exec,
                    miscdirs],
            target="../dist")

env.InstallAs(target='../dist/gufg.py', source='../src/scripts/gufg.py')
