import os
from envhelper import *
from confighelper import *

def AddVariables(vars):
    vars.Add( "MSVC_USE_SCRIPT", "Name of the vcvarsall.bat script to read to initialize the environment, or False to disable reading", 
	True, None, ConvertStrBool )
    vars.Add( "MSVC_PATH", "MS Visual Studio installation path (set if autodetection fails)", GetOSEnv('VCINSTALLDIR') )
    vars.Add( "MSVC_IDE_PATH", "MSVC common installation path (set if autodetection fails)", GetOSEnv('DevEnvDir') )
    vars.Add( "WINSDK_PATH", "MS VC SDK installation path (set if autodetection fails)", GetOSEnv('WindowsSdkDir') )

def SetupEnv(env):

    usescript = env.get('MSVC_USE_SCRIPT',True)

    msvc = env.get('MSVC_PATH','')

    if msvc == '' and not usescript:
	print "MSVC_PATH not set. Please make sure that VCINSTALLDIR is set by running 'vcvarsall.bat'"
	print "in your VC8 installation dir or set MSVC_PATH to scons."
	return

    winsdk = env.get('WINSDK_PATH','')
    if winsdk == '' and not usescript:
	print "WINSDK_PATH not set. Please make sure that WindowsSdkDir is set by running 'vcvarsall.bat'"
	print "in your VC8 installation dir or set WINSDK_PATH to scons."
	return
    
    devenv = env.get('MSVC_IDE_PATH','')

    if env['TARGET_ARCH'] == 'amd64':
	archDir = '/amd64'
	sdkArchDir = '/x64'
    else:
	archDir = ''
	sdkArchDir = ''

    if msvc != '':
	env.AppendENVPath('LIB',     os.path.join(env["MSVC_PATH"],"lib"+archDir) )
	env.AppendENVPath('LIBPATH', os.path.join(env["MSVC_PATH"],"lib"+archDir) )
	env.AppendENVPath('INCLUDE', os.path.join(env['MSVC_PATH'],'include') )
	env.AppendENVPath('PATH',    os.path.join(env['MSVC_PATH'],'bin'+archDir) )

    if winsdk != '':
	env.AppendENVPath('LIB',     os.path.join(env["WINSDK_PATH"],"lib"+sdkArchDir) )
	env.AppendENVPath('INCLUDE', os.path.join(env['WINSDK_PATH'],'include') )
	env.AppendENVPath('PATH',    os.path.join(env['WINSDK_PATH'],'bin'+sdkArchDir) )

    if devenv != '':
        env.AppendENVPath('PATH',    devenv)
