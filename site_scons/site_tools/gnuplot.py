"""
Builder for gnuplot graphs.
"""
#
# @project     SCons gnuplot helper toolbox
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date:  $
# @version     $Id:  $
#
# This tool adds support for .gnu or .gp gnuplot files. 
# If the gnuplot files contain one or more 'set output' statements, variant_dir must not be used with duplicate=0.
# 
# Gnuplot is always called using the directory of the source file as working dir, i.e. all paths must be relative
# to the top source file.
# 
# TODO the scanner for data files is a very simple hack, should be improved.
#
# TODO it would be *really* nice, if this would work together with LaTeX, so that \includegraphics{} creates a dependency to gnuplot
#      and the pdf-figure is generated automagically, if only a .gnu file but no other file exists in the source-dir.
#
# Copyright (C) 2011 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#

import os.path
import re

from SCons.Script import *

output_re = re.compile("^[ \t]*set[ \t]+output[ \t]+[\"'](.*)[\"']", re.MULTILINE)

include_re = re.compile("^[ \t]*(load|call)\s+[\"'](.*)[\"']", re.MULTILINE)
plot_re = re.compile("^[ \t]*plot\s+.*[\"'](.*(.dat|.csv))[\"']", re.MULTILINE)

def GnuplotGenerator(source, target, env, for_signature):
    # TODO set term depending on target[0].suffix
    
    return "$GNUPLOTCOMSTR -e 'set term pdfcairo' ${SOURCE.file}"

def GnuplotEmitter(target, source, env):
    # add 'set output <file> as targets, relative to source-dir, replaces target if present
    targets = []
    for i in range(len(source)):
	src = source[i]
        found = output_re.findall(src.get_text_contents())
	output = map(lambda(f) : env.File(f[0], src.dir), found)
	
	# TODO for epslatex term type, we need to add the .eps file for every .tex file too!

        if len(output) > 0:
	    targets.extend( output )
	else:
	    targets.append( target[i] )
    
    return (targets, source)

def GnuplotScanFile(workdir, node, env):
    # srcnode.exists is a small workaround for 2.1.0
    if not node.exists() and not node.srcnode().exists():
	return []

    found = []
    contents = node.get_text_contents()

    # scan for 'load', 'call', and 'plot', ignore comments
    for match in include_re.findall(contents):
	gnufile = env.File( match[1], workdir )
	found.append( gnufile )
	found.extend( GnuplotScanFile( workdir, gnufile, env ) )

    for match in plot_re.findall(contents):
	datafile = env.File( match[0], workdir )
	found.append( datafile )

    return found

    
def GnuplotScannerFunc(node, env, path):
    # We assume here, that the working dir is SOURCE.dir
    return GnuplotScanFile(node.dir, node, env) 

GnuplotScanner = Scanner(
	function = GnuplotScannerFunc, name = "Gnuplot", recursive = 0
)

GnuplotBuilder = Builder(
	generator = GnuplotGenerator,
	suffix = '.pdf', src_suffix = [ '.gnu', '.gp' ], single_source = 1,
	source_scanner = GnuplotScanner, emitter = GnuplotEmitter
)

GnuplotPDFBuilder = Builder(
        action = "$GNUPLOTPDFCOMSTR",
	suffix = '.pdf', src_suffix = [ '.gnu', '.gp' ], single_source = 1,
	source_scanner = GnuplotScanner, emitter = GnuplotEmitter
)

GnuplotPNGBuilder = Builder(
        action = "$GNUPLOTCOMSTR -e 'set term pngcairo' ${SOURCE.file}",
	suffix = '.png', src_suffix = [ '.gnu', '.gp' ], single_source = 1,
	source_scanner = GnuplotScanner, emitter = GnuplotEmitter
)

def generate(env, **kw):
    """Add gnuplot builders to the construction environment.
       
       Usage: Use env.GnuplotEPS("file.gnu") to generate a eps file. 
      
       If you include 'set output <filename>' in your .gnu script, you must not use 'duplicate=0' for variant-dirs.
    """

    # go into the (variant) source dir, load files relative to source file, generate to abs path.
    env['GNUPLOTCOM'] = '$GNUPLOT -e "cd \'${SOURCE.dir}\'; set output \'${TARGET.abspath}\'" '
    env['GNUPLOTCOMSTR'] = '$GNUPLOTCOM ${SOURCE.file}'

    env['GNUPLOTPDFCOMSTR'] = '$GNUPLOTCOM -e "set term pdfcairo" ${SOURCE.file}'

    # register a general purpose builder, that determines the terminal based on the target file, or for .gnu plot files containing output commands
    env['BUILDERS']['Gnuplot'] = GnuplotBuilder

    # TODO handle eps builder with special care, creates .tex and .eps files
    #env['BUILDERS']['GnuplotEPS'] = GnuplotEPSBuilder
    env['BUILDERS']['GnuplotPNG'] = GnuplotPNGBuilder
    env['BUILDERS']['GnuplotPDF'] = GnuplotPDFBuilder

    try:
        bld = env['BUILDERS']['PDF']
        bld.add_action('.gnu', '$GNUPLOTPDFCOMSTR')
        bld.add_action('.gp', '$GNUPLOTPDFCOMSTR')
    except:
        pass


def exists(env):
    return exists([env.subst('$GNUPLOT')])

