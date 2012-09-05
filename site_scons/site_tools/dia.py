"""
Builder for ReST format doc files.
"""
#
# @project     SCons dia file helper toolbox
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date:  $
# @version     $Id:  $
#
# Copyright (C) 2011 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#

from SCons.Script import *

DiaEPSBuilder = Builder(
        action = "$DIAEXPORTCOMSTR",
	suffix = '.eps', src_suffix = '.dia', single_source = 1
)

DiaPNGBuilder = Builder(
        action = "$DIAEXPORTCOMSTR",
	suffix = '.png', src_suffix = '.dia', single_source = 1
)

DiaSVGBuilder = Builder(
        action = "$DIAEXPORTCOMSTR",
	suffix = '.svg', src_suffix = '.dia', single_source = 1
)

def generate(env, **kw):
    """Add dia export builders to the construction environment."""

    env['DIACOM'] = '$DIA'
    env['DIAEXPORTCOMSTR'] = '$DIACOM $SOURCE -e $TARGET'

    # TODO default builder that expects a target extension
    #env['BUILDERS']['Dia'] = 

    env['BUILDERS']['DiaEPS'] = DiaEPSBuilder
    env['BUILDERS']['DiaPNG'] = DiaPNGBuilder
    env['BUILDERS']['DiaSVG'] = DiaSVGBuilder

    try:
        bld = env['BUILDERS']['PDF']
        bld.add_action('.dia', '$DIAEXPORTCOMSTR')
    except:
        pass

    try:
	bld = env['BUILDERS']['EPS']
        bld.add_action('.dia', '$DIAEXPORTCOMSTR')
    except:
	env['BUILDERS']['EPS'] = DiaEPSBuilder


def exists(env):
    return exists([env.subst('$DIA')])

