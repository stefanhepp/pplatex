import os
import platform
from envhelper import *

def AddVariables(vars):
    gnuplot = FindTool('gnuplot')

    vars.Add('GNUPLOT', 'Path to the gnuplot executable.', gnuplot)


def SetupEnv(env):
    
    gnuplot = env.get('GNUPLOT',None)
    if gnuplot is None:
        env['GNUPLOT'] = 'gnuplot'
    env.Tool('gnuplot')

