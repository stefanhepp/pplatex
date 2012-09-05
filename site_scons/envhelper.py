import os
from SCons.Script import *

def GetArchBits(env):
    """Get the number of bits for the target architecture"""
    if env['TARGET_ARCH'] == "amd64" or env['TARGET_ARCH'] == "x86_64" or env['TARGET_ARCH'] == "ia64":
	return "64"
    return "32"

def FindToolPath(program, default=None):
    """Get the path for a program from the OS environment."""
    file=WhereIs(program)
    if file is None:
	return default
    return os.path.dirname(file)

def FindTool(program, default=None):
    """Get the path of a program binary from the OS environment."""
    file=WhereIs(program)
    if file is None:
	return default
    return file

def GetOSEnv(name, default=None):
    """Get a variable from the OS environment."""
    try:
	return os.environ[name]
    except:
	return default

def Dirname(dir):
    if dir is None:
	return None
    return os.path.dirname(dir)

