import os
from envhelper import *

def AddVariables(vars):
    vars.Add( "DMDPATH", "DMD Install Path", "/opt/dmd/dmd" )

def SetupEnv(env):
    # TODO check if dmd is installed, check for version .. 
    env['DC'] = 'ldc'
    env['LINK'] = 'ldc'
    env['LINKCOM'] = '$LINK -of$TARGET $SOURCES $DFLAGS $LINKFLAGS $LINKLIBFLAGS -L="-ltango-user-ldc"'
    env['LIBS'] = [ 'tango' ]

