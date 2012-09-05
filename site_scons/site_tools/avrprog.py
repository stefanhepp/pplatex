"""
Tool to add support for common programmer tools.
"""
#
# @project     SCons avr programmer extension
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date: 2007-08-05 18:15:52 +0200 (Son, 05 Aug 2007) $
# @version     $Id: avrgcc.py 36 2007-08-05 16:15:52Z stefan $
#
# TODO make this whole stuff more platform/compiler independent
#
# Copyright (C) 2007 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#

import os

from SCons.Script import *

def AvrUispProgrammer(target, source, env):
    """Run an uisp programmer. Target is an arbitrary name to identify the build target.
    """
    
    # TODO upload 'HEXSUFFIX' to flash, 'EEPSUFFIX' to eeprom, verify data option

    cmd = env.subst("uisp " + env.get("AVRPROGFLAGS") + " -dprog=" + env.get("AVRPROGFORMAT") + " -dpart=" + env.get("MCU"))

    print cmd + " --segment=flash --erase"
    rs = os.system(cmd + " --segment=flash --erase")
    if rs != 0:
	print "Error erasing flash!"
	return rs

    print cmd + " --segment=flash --upload if=" + str(source[0])
    rs = os.system(cmd + " --segment=flash --upload if=" + str(source[0]) )
    if rs != 0:
	print "Error uploading flash!"
	return rs

    return 0

def AvrProgrammer(target, source, env):
    """Run programmer tool, upload files given in sources. Target is interpreted 
    depending on the used programmer as target board identifier or as arbitrary name."""
    
    if env["AVRPROG"] == "uisp":
	return AvrUispProgrammer(target, source, env)
    else:
	print "Programmer " + env["AVRPROG"] + " unknown!"
	return 1

AvrUploadBuilder = Builder( action = AvrProgrammer )

def generate(env, **kw):
    """Add Builders to upload data."""

    env['BUILDERS']['Upload'] = AvrUploadBuilder

    env.SetDefault( AVRFORMAT = 'srec',
		    AVRPROG = 'uisp',
		    AVRPROGFORMAT = 'dasa2',
		    AVRPROGDEV = '/dev/ttyS0',
		    AVRPROGFLAGS = '-dserial=$AVRPROGDEV'
		   )

def exists(env):
    return env.Detect('$AVRPROG')


