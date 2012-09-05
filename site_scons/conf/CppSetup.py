import os
import platform
import MSVCSetup
from envhelper import *

def AddVariables(vars):
    if platform.system() == "Windows":
	MSVCSetup.AddVariables(vars)

def SetupFlags(env, addReleaseVars=True):
    """Setup various common C++ defines, such as compiler, platform, release, ...
    """
    # TODO use Configure here, create config.h file?

    if env['PLATFORM'] == "win32":
	env.AppendUnique( CPPDEFINES=['_WIN32'] )

    if addReleaseVars:
	release = env.get('RELEASE',None)
	if release is not None:
	    env.AppendUnique( CPPDEFINES=[('RELEASE',release)] )

	name = env.get('RELEASE_NAME', None)
	version = env.get('RELEASE_VERSION', None)

	if name is not None:
	    env.AppendUnique( CPPDEFINES=[('RELEASE_NAME', name)] )
	if version is not None:
	    env.AppendUnique( CPPDEFINES=[('RELEASE_VERSION', version)] )


def SetupEnv(env, runtime="default", console=False, exceptions=True, omitFP=True, manifestDir=None, pdb=None, addReleaseVars=True):
    """Setup the C++ compiler environment.
       Parameters:
       - runtime	Runtime version to use: one of 'default', 'mt', 'mt-lib' 
	                (Default, Multithreaded, statically linked, Multithreaded, shared lib)
       - console	Compile as console application
       - exceptions	Use C++ exceptions
       - omitFP		Omit frame pointers
       - manifestDir    Directory to store manifest files. If set, manifests will be created and added
			to programs and libraries. 
       - pdb		Name of pdb file. Will be generated in debug mode if set.
       - addReleaseVars	Add RELEASE, RELEASE_NAME and RELEASE_VERSION as defines from env.
    """

    # TODO not not call setupflags if it has already been set for env
    SetupFlags(env, addReleaseVars)

    release = env.get('RELEASE',None)

    if env['CC'] == 'cl':
	MSVCSetup.SetupEnv(env)

	def IgnoreLibs(env, libs):
	    env.AppendUnique( LINKFLAGS=[ '/NODEFAULTLIB:'+lib+'.lib' for lib in libs ] )

	if release == 'debug':
	    env.AppendUnique( CPPFLAGS=['/W4', '/Z7', '/Od'])

	    env.AppendUnique( LINKFLAGS=['/DEBUG'] )

	    if runtime == 'mt-lib':
		env.AppendUnique( CPPFLAGS=['/MDd'] )
		IgnoreLibs(env, ['libc', 'libcmt', 'msvcrt', 'libcd', 'libcmtd' ] )
	    elif runtime == 'mt':
		env.AppendUnique( CPPFLAGS=['/MTd'] )
		IgnoreLibs(env, ['libc', 'libcmt', 'msvcrt', 'libcd', 'msvcrtd' ] )

	    if pdb is not None:
		env['PDB'] = pdb

	# end Debug flags

	if release == 'release':
	    env.AppendUnique( CPPFLAGS=['/W3', '/O2', '/Oi', '/GL'] )

	    env.AppendUnique( LINKFLAGS=['/INCREMENTAL:NO', '/LTCG'] )

	    if omitFP:
		env.AppendUnique( CPPFLAGS=['/Oy'] )

	    if runtime == 'mt-lib':
		env.AppendUnique( CPPFLAGS=['/MD'] )
		IgnoreLibs(env, ['libc', 'libcmt', 'msvcrtd', 'libcd', 'libcmtd' ] )
	    elif runtime == 'mt':
		env.AppendUnique( CPPFLAGS=['/MT'] )
		IgnoreLibs(env, ['libc', 'msvcrt', 'msvcrtd', 'libcd', 'libcmtd' ] )

	# end Release flags
	
	if exceptions:
	    env.AppendUnique( CPPFLAGS=['/EHsc'] )
	
	env.AppendUnique( CPPFLAGS=['/nologo'] )

	if console:
	    env.AppendUnique( CPPDEFINES=['_CONSOLE'] )
	    env.AppendUnique( LINKFLAGS=['/SUBSYSTEM:CONSOLE'] )
	

	if manifestDir is not None:
	    # TODO add manifest as sideeffect() and clean() using an emitter 
	    manifestfile = os.path.join(manifestDir, '${TARGET.file}.manifest')
	    env.AppendUnique( LINKFLAGS=['/MANIFEST', '/MANIFESTFILE:'+manifestfile] )
	    env['LINKCOM'] = [env['LINKCOM'], 'mt.exe -nologo -manifest '+manifestfile+' -outputresource:$TARGET;1']
	    env['SHLINKCOM'] = [env['SHLINKCOM'], 'mt.exe -nologo -manifest '+manifestfile+' -outputresource:$TARGET;2']


    # TODO for MinGW compiler, use manifest:
    # http://www.scons.org/wiki/EmbedManifestIntoTarget
    # - create the manifest-file (once) into manifestDir/msvcrt.manifest as env.Manifest(manifestDir/app.manifest)
    # - create .rc file 
    #   fout = open(manifestDir+"/msvcr.rc", "w")
    #   fout.write("""
    #   #include "winuser.h"
    #   1 RT_MANIFEST  msvcrt.manifest
    #   """)
    #   fout.close()
    # - compile rc file (RC-builder?, windres --input manifestDir/msvcr.rc --output manifestDir/msvcrc.o)
    # - link with manifestDir/msvcrc.o, add as dependency

    else:
	env.AppendUnique( CPPFLAGS=['-Wall', '-W', '-Wconversion'] )
	if release == 'debug':
	    env.AppendUnique( CPPFLAGS=['-g'] )
	elif release == 'release':
	    env.AppendUnique( CPPFLAGS=['-O3'] )
    
