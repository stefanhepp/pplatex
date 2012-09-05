"""
Misc file and helper builders.
"""
#
# @project     SCons misc file helper toolbox
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date:  $
# @version     $Id:  $
#
# Copyright (C) 2007 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#
import os
from SCons.Script import *

def FileCopyTo(env, target, source):
    """Copy multiple source files to a target directory."""
    if isinstance(target,str):
        tdir = Dir(target)
    else:
        tdir = target

    if isinstance(source,dict):
        return dict( (key,FileCopyTo(env, tdir, src)) for key,src in source.iteritems())

    try:
        return map( lambda x: FileCopyTo(env, tdir, x), source )
    except TypeError:

        if isinstance(source,str):
            filename = source
        else:
            filename = source.path

        filename = os.path.basename(filename)
        tfile = tdir.File(filename)

        return env.Command(tfile, source, Copy('$TARGET', '$SOURCE'))


def generate(env, **kw):
    """Add file helper builders to the construction environment."""

    env['BUILDERS']['FileCopy'] = Builder( action = Copy("$TARGET","$SOURCE") )
    env['BUILDERS']['FileMove'] = Builder( action = Move("$TARGET","$SOURCE") )
    env['BUILDERS']['FileDelete'] = Builder( action = Delete("$TARGET") )
    env['BUILDERS']['FileTouch'] = Builder( action = Touch("$TARGET") )

    env['BUILDERS']['Mkdir'] = Builder( action = Mkdir("$TARGET") )

    env.AddMethod(FileCopyTo)
    

def exists(env):
    return 1

