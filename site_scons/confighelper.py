import os
import platform
from SCons.Script import *

def ConvertStrBool(value, env=None):
    if value is None:
	return False

    if SCons.Util.is_String(value):
	if value == '' or value == 'False' or value == 'false':
	    return False
	if value == 'True' or value == 'true':
	    return True
	return value

    return value


def InitVariables(argname='--conffile', option='config', release=False):
    """Create a new Variables object, add some default options."""

    system = platform.system().lower()
    pfile = 'config-'+system+'.py'
    if system != '' and os.path.isfile(pfile):
	default = pfile
    else:
	default = 'config.py'
    
    AddOption(argname, default=default,
          dest=option, type='string', nargs=1, action='store', metavar='FILE',
          help='Set the configuration filename')

    vars = Variables(GetOption(option))

    if release:
	vars.Add(EnumVariable('RELEASE', 'Set the release type', 'release', allowed_values=('release','debug')))

    return vars

def CreateEnv(vars):
    # TODO make optional, use config variables if set
    env = {'PATH':os.environ['PATH']}
    if platform.system().lower() == "windows":
        env['TMP']  = os.environ['TMP']
    return env

def LoadReleaseFile(env, filename):

    rfile = file( filename, "rU" )
    for line in rfile:
	line = line.strip()
	if len(line) == 0:
	    continue
	if line[0] == "#":
	    continue
	sep = line.find('=')
	if sep == -1:
	    continue
	name = line[:sep].strip()
	value = line[sep+1:].strip()

	if name == 'VERSION':
	    if env['RELEASE'] == 'debug':
		value += "-debug"

	if name in ( 'NAME', 'VERSION' ):
	    env['RELEASE_'+name] = value

    rfile.close()
