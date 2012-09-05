"""
Builder for music score files.
"""
#
# @project     SCons music score helper toolbox
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date:  $
# @version     $Id:  $
#
# Copyright (C) 2011 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#

from SCons.Script import *

MScorePDFBuilder = Builder(
        action = "MSCORECOM",
	suffix = '.pdf', src_suffix = [ '.mscx','.mscz','.mxl','.xml','.mid','.midi' ], single_source = 1
)
MScoreMIDIBuilder = Builder(
        action = "MSCORECOM",
	suffix = '.midi', src_suffix = [ '.mscx','.mscz','.mxl','.xml' ], single_source = 1
)
MScoreLyBuilder = Builder(
        action = "MSCORECOM",
	suffix = '.ly', src_suffix = [ '.mscx','.mscz','.mxl','.xml','.mid','.midi' ], single_source = 1
)
MScoreOggBuilder = Builder(
        action = "MSCORECOM",
	suffix = '.pdf', src_suffix = [ '.mscx','.mscz','.mxl','.xml','.mid','.midi' ], single_source = 1
)

LilypondBuilder = Builder(
        action = "$LILYPONDCOM",
	suffix = '.pdf', src_suffix = '.ly', single_source = 1
)

def generate(env, **kw):
    """Add notation score builders to the construction environment."""

    env['MSCORECOM'] = '$MSCORE $SOURCE -o $TARGET'
    env['LILYPONDCOM'] = '$LILYPOND -o $TARGET $SOURCE'

    env['BUILDERS']['MScorePDF']  = MScorePDFBuilder
    env['BUILDERS']['MScoreMIDI'] = MScoreMIDIBuilder
    env['BUILDERS']['MScoreLy']   = MScoreLyBuilder
    env['BUILDERS']['MScoreOgg']  = MScoreOggBuilder

    env['BUILDERS']['Lilypond'] = LilypondBuilder

    env.SetDefault(MSCORE = 'mscore')
    env.SetDefault(LILYPOND = 'lilypond')

    try:
        bld = env['BUILDERS']['PDF']
	bld.add_action('.mscx', '$MSCORECOM')
	bld.add_action('.mscz', '$MSCORECOM')
	bld.add_action('.mxl',  '$MSCORECOM')
	bld.add_action('.ly', '$LILYPONDCOM')
    except:
	pass

def exists(env):
    return exists([env.subst('$MSCORE'),env.subst('$LILYPOND')])

