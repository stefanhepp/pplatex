"""
Tool to add AVR-GCC compiler support and some builders to create output files.
"""
#
# @project     SCons avr-gcc extension
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date: 2009-10-31 03:44:22 +0100 (Sat, 31 Oct 2009) $
# @version     $Id: avr.py 109 2009-10-31 02:44:22Z stefan $
#
# TODO make this whole stuff more platform/compiler independent
# TODO make obj-copy builder work correcty with more than one source .elf file
# TODO use -fwhole-program per default; needs a way to skip object compilation
#
# Copyright (C) 2007 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#

from SCons.Script import *

def AvrProgEmitter(target, source, env):

    exe = env.FindIxes(target, "PROGPREFIX", "PROGSUFFIX")
    if not exe:
	raise SCons.Errors.UserError, "An executable should have exactly one target with the suffix: '%s'" % env.subst("$PROGSUFFIX")

    target.append(env.ReplaceIxes(exe, "PROGPREFIX", "PROGSUFFIX", "PROGPREFIX", "MAPSUFFIX" ))

    return (target, source)

AvrHexBuilder = Builder( 
	    action = "$OBJCOPY $OCFLAGS -O $AVRFORMAT $SOURCE $TARGET",
	    suffix = '${HEXSUFFIX}', src_suffix = '${PROGSUFFIX}', single_source = 0
	)

AvrEepBuilder = Builder( 
	    action = "$OBJCOPY $EEPFLAGS -O $AVRFORMAT $SOURCE $TARGET",
	    suffix = '${EEPSUFFIX}', src_suffix = '${PROGSUFFIX}', single_source = 0
	)

def generate(env, **kw):
    """Add Builders and construction variables for avr-gcc to an Environment."""

    env['BUILDERS']['HexObject'] = AvrHexBuilder
    env['BUILDERS']['EepObject'] = AvrEepBuilder

    env.Append(PROGEMITTER = [AvrProgEmitter])
    env['CC'] = "avr-gcc"
    env['AS'] = "avr-as"
    env['SHLINK'] = "avr-ld"
    env['LINK'] = "avr-gcc"
    env['OBJCOPY'] = "avr-objcopy"
    env['ASFLAGS'] = "-mmcu=${MCU} -Wa,-mmcu=${MCU} -ggdb"
    env['LINKFLAGS'] = "-mmcu=${MCU} -Wl,-Map=${TARGET.base}${MAPSUFFIX}"
    env['CFLAGS'] = "${WARNLEVEL} -Os -mmcu=${MCU} -ggdb"
    env['CPPPATH'] = [ "#${SRCDIR}", ".", "#${SRCDIR}/usbrc" ]

    env['PROGSUFFIX'] = '.elf'
    env['EEPSUFFIX'] = '.eep'
    env['HEXSUFFIX'] = '.hex'
    env['MAPSUFFIX'] = '.map'

    env.SetDefault( AVRFORMAT = 'srec',
		    EEPFLAGS = '-j .eeprom --change-section-lma .eeprom=0 --set-section-flags=.eeprom="alloc,load"',
		    OCFLAGS = '',
		   )

def exists(env):
    return env.Detect('avr-gcc')


