import os
from envhelper import *
from SCons.Script import *

def AddVariables(vars, glew=False):
    if glew:
	vars.Add( PathVariable('GLEWPATH', 'Path under which GLEW is installed', None, PathVariable.PathAccept) ) 


def SetupEnv(env, glu=False, glut=False, glew=False):
    """Setup OpenGL libraries """

    if glew:
	glu = True

    if env['PLATFORM'] == "win32":
	env.Append( LIBS=['opengl32'] )
	if glu:
	    env.Append( LIBS=['glu32'] )
	if glut:
	    env.Append( LIBS=['glut32'] )
	if glew:
	    env.Append( LIBS=['glew32'] )
    else:
	env.Append( LIBS=['GL'] )
	if glu:
	    env.Append( LIBS=['GLU'] )
	if glut:
	    env.Append( LIBS=['glut'] )
	if glew:
	    env.Append( LIBS=['GLEW'] )

    if glew:
	glewpath = env.get('GLEWPATH',None)

	if glewpath is not None:
	    env.Append( CPPPATH=[os.path.join(glewpath,'include')] )
	    env.Append( LIBPATH=[os.path.join(glewpath,'lib')] )
	

