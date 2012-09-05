import os
from envhelper import *

def AddVariables(vars):
    vars.Add( "SDL_INCLUDE", "SDL include path", "/usr/include/SDL" )
    vars.Add( "SDL_LIBPATH", "SDL library path", "/usr/lib" )

def SetupEnv(env):
    """Setup SDL library dependencies.

    To compile correctly, you need to use 
      CppSetup.SetupEnv(env, runtime="mt-lib", console=True, manifestDir="obj")
    """

    env.Append( LIBS=['SDL','SDLmain'], LIBPATH=['$SDL_LIBPATH'], CPPPATH=['$SDL_INCLUDE'] )
    env.Append( CPPDEFINES=[('_GNU_SOURCE','1'), 'REENTRANT'] )

