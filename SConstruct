import os
from conf import CppSetup
from conf import LatexSetup

vars = InitVariables(release=True)
CppSetup.AddVariables(vars)
LatexSetup.AddVariables(vars)

vars.Add(PathVariable('PCREPATH', 'Path of pcre library installation path.',None,PathVariable.PathAccept));

env = DefaultEnvironment(variables=vars, ENV=CreateEnv(vars))
env.Tool('file')

LoadReleaseFile(env, 'RELEASE')

CppSetup.SetupEnv(env, console=True)
LatexSetup.SetupEnv(env)

if env['CC'] == 'cl':
    env.Append( CPPFLAGS=['/EHsc'] )

if env.get('PCREPATH', None) is not None:
    env.Append( LIBPATH=[ env['PCREPATH']+'/lib' ] )
    env.Append( CPPPATH=[ env['PCREPATH']+'/include' ] )
env.Append( LIBS=['pcreposix'] )

app = SConscript("src/SConscript", variant_dir="obj", duplicate=0, exports="env")
app2 = env.FileCopy("bin/ppdflatex"+env['PROGSUFFIX'], app)

VariantDir('tmp', 'test', duplicate=1)
pdf = PDF('tmp/test.tex')
pdf2 = PDF('tmp/test_paren.tex')

Alias('app', app2)
Alias('test', [pdf,pdf2] )

Default('app')

Help(vars.GenerateHelpText(env))

