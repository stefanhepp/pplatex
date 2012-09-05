"""
Builder for ReST format doc files.
"""
#
# @project     SCons rst file helper toolbox
# @author      Stefan Hepp, stefan@stefant.org
# @date        $Date:  $
# @version     $Id:  $
#
# TODO if rst2pdf does not exist, use rst2latex + Latex-PDF-builder
# TODO get tool options from environment
#
# Copyright (C) 2009 Stefan Hepp
# License: MIT
# See 'COPYRIGHT.txt' for copyright and licensing information.
#

from SCons.Script import *

RstPdfBuilder = Builder(
        action = "$RST2PDFCOMSTR",
	suffix = '.pdf', src_suffix = '.txt', single_source = 1
)

RstHtmlBuilder = Builder(
        action = "$RST2HTMLCOMSTR",
	suffix = '.html', src_suffix = '.txt', single_source = 1
)

RstLatexBuilder = Builder(
        action = "$RST2LATEXCOMSTR",
	suffix = '.tex', src_suffix = '.txt', single_source = 1
)

def generate(env, **kw):
    """Add rst builders to the construction environment."""

    env['RST2PDFCOM'] = 'rst2pdf'
    env['RST2HTMLCOM'] = 'rst2html'
    env['RST2LATEXCOM'] = 'rst2latex'
    env['RST2PDFCOMSTR'] = '$RST2PDFCOM $SOURCE -o $TARGET'
    env['RST2HTMLCOMSTR'] = '$RST2HTMLCOM $SOURCE $TARGET'
    env['RST2LATEXCOMSTR'] = '$RST2LATEXCOM $SOURCE $TARGET'

    env['BUILDERS']['RstPdf'] = RstPdfBuilder
    env['BUILDERS']['RstHtml'] = RstHtmlBuilder
    env['BUILDERS']['RstLatex'] = RstLatexBuilder

    try:
        bld = env['BUILDERS']['PDF']
	bld.add_action('.txt', '$RST2PDFCOMSTR')
    except:
	pass

def exists(env):
    return exists(['rst2html','rst2pdf'])

