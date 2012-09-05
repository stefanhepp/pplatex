import os
import platform
from envhelper import *

def AddVariables(vars):
    dia = FindTool('dia')

    vars.Add('DIA', 'Path to the Dia executable.', dia)


def SetupEnv(env):
    
    dia = env.get('DIA',None)
    if dia is None:
        env['DIA'] = 'dia'
    env.Tool('dia')

