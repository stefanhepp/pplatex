import os
from conf import CppSetup
from conf import LatexSetup

vars = InitVariables(release=True)
CppSetup.AddVariables(vars)
LatexSetup.AddVariables(vars)

vars.Add(PathVariable('PCREPATH', 'Path of pcre library installation path.',None,PathVariable.PathAccept));
vars.Add(PathVariable('DESTDIR', 'Destination directory.', None, PathVariable.PathAccept))

env = DefaultEnvironment(variables=vars, ENV=CreateEnv(vars))
env.Tool('file')

LoadReleaseFile(env, 'RELEASE')

CppSetup.SetupEnv(env, console=True)
LatexSetup.SetupEnv(env)

if env['CC'] == 'cl':
    env.Append( CPPFLAGS=['/EHsc'] )
    # TODO if we link statically with PCRE on windows with MSVC, set PCRE_STATIC automatically, or make this an config option.
    #env.Append( CPPDEFINES=['PCRE_STATIC'] )

if env.get('PCREPATH', None) is not None:
    env.Append( LIBPATH=[ env['PCREPATH']+'/lib' ] )
    env.Append( CPPPATH=[ env['PCREPATH']+'/include' ] )
env.Append( LIBS=['pcreposix'] )

# Build the main program and copy it with different filenames
app = SConscript("src/SConscript", variant_dir="obj", duplicate=0, exports="env")
latex = app
pdftex = env.FileCopy("bin/ppdflatex"+env['PROGSUFFIX'], app)
if env['PLATFORM'] == 'posix':
    # On linux, we use the shell script for luatex that supports a user config
    luatex = env.FileCopy("bin/ppluatex", "src/ppluatex")
else:
    # Otherwise, we just copy the binary again.
    luatex = env.FileCopy("bin/ppluatex"+env['PROGSUFFIX'], app)


# For testing, run the .tex files in test/ through pplatex inside a temp directory
VariantDir('tmp', 'test', duplicate=1)
pdf = PDF('tmp/test.tex')
pdf2 = PDF('tmp/test_paren.tex')

# Install
prefix = env.get('DESTDIR', '') + '/usr'
bindir = prefix + '/bin'
env.Install(bindir, [latex, pdftex, luatex])
Alias('install', bindir)

# Setup aliases and default target
Alias('app', [pdftex,luatex] )
Alias('test', [pdf,pdf2] )

Default('app')

Help(vars.GenerateHelpText(env))

