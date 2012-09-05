import os
import platform
from envhelper import *

def UseLatexSync(env):
    return env.get('LATEX_SYNC', 1)

def AddVariables(vars):
    latex = FindToolPath('latex')
    pplatex = FindToolPath('pplatex')

    vars.Add('LATEX_PATH', 'Path to the latex binary tools.', latex)
    vars.Add(BoolVariable('LATEX_SYNC', 'Enable inverse-search for generated files', 1))
    vars.Add('PPLATEX','Path to the pplatex binaries, or "auto" to use pplatex only if found in the current path.', pplatex)
    vars.Add('PPLATEX_OPT','Options for pplatex', '')

def SetupEnv(env):
    
    latex = env['LATEX_PATH']
    if latex is not None:
	env.PrependENVPath('PATH', latex)
	env.Tool('tex')
	env.Tool('gs')
	env.Tool('latex')
	env.Tool('dvipdf')
	env.Tool('dvips')
	env.Tool('pdflatex')
	env.Tool('pdftex')

    if UseLatexSync(env):
	env['LATEXFLAGS'] = env['LATEXFLAGS'] + ' -synctex=1'
	env['PDFLATEXFLAGS'] = env['PDFLATEXFLAGS'] + ' -synctex=1'

    pplatex = env.get('PPLATEX',None)

    if pplatex is None or pplatex == '':
	return

    # TODO if latex tools are in a non-standard path, add -c <exe> -- to arguments, provide option to set latex path

    if pplatex == "auto":
	if env.Detect(['pplatex','ppdflatex']):
	    env['LATEX'] = 'pplatex'
	    env['PDFLATEX'] = 'ppdflatex'

    else:
	env['LATEX'] = os.path.join(pplatex,'pplatex')
	env['PDFLATEX'] = os.path.join(pplatex,'ppdflatex')

    opts = env['PPLATEX_OPT']

    if opts != '':
	env['LATEX'] = env['LATEX'] + ' ' + opts + ' -- '
	env['PDFLATEX'] = env['PDFLATEX'] + ' ' + opts + ' -- '

