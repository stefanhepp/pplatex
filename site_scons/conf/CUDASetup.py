import os
import platform
import SCons
from envhelper import *

def AddVariables(vars):
    vars.Add( "CUDA_TOOLKIT_PATH", "CUDA toolkit path", Dirname(GetOSEnv('CUDA_BIN_PATH')) )
    vars.Add( "CUDA_SDK_PATH", "CUDA SDK path", GetOSEnv('NVSDKCOMPUTE_ROOT') )
    vars.Add( "NVCCFLAGS", "CUDA nvcc flags", "" )

def SetupEnv(env, optional=False, cutil=False):
    """Setup CUDA environment. 
    """

    env['NVCCFLAGS'] = SCons.Util.CLVar(env['NVCCFLAGS'])

    # find CUDA Toolkit path and set CUDA_TOOLKIT_PATH
    try:
	cudaToolkitPath = env['CUDA_TOOLKIT_PATH']
    except:
	cudaToolkitPath = None
    if cudaToolkitPath == None:
	cudaToolkitPath == FindCudaToolkit()
	env['CUDA_TOOLKIT_PATH'] = cudaToolkitPath
    if cudaToolkitPath == None:
	print "scons: Cannot find the CUDA Toolkit path. Please set env['CUDA_TOOLKIT_PATH'] to point to your Toolkit path"
	return

    # find CUDA SDK path and set CUDA_SDK_PATH
    try:
	cudaSDKPath = env['CUDA_SDK_PATH']
    except:
	cudaSDKPath = None
    if cudaSDKPath == None:
	cudaSDKPath = FindCudaSDK()
	env['CUDA_SDK_PATH'] = cudaSDKPath
    if cudaSDKPath == None and not optional:
	print "scons: Cannot find the CUDA SDK path. Please set env['CUDA_SDK_PATH'] to point to your SDK path."
	return

    # cuda libraries
    if env['PLATFORM'] == 'posix':
	cudaSDKSubLibDir = '/linux'
    elif env['PLATFORM'] == 'darwin':
	cudaSDKSubLibDir = '/darwin'
    else:
	cudaSDKSubLibDir = ''
    cudaSDKLibDir = '/lib'

    if os.path.exists(cudaToolkitPath + '/bin64'):
	cudaLibDir = '/lib64'
	cudaBinDir = '/bin64'
    else:
	cudaLibDir = '/lib'
	cudaBinDir = '/bin'

    # add nvcc to PATH, setup Toolkit
    env.PrependENVPath('PATH', cudaToolkitPath + cudaBinDir)

    env['ENV']['CUDA_BIN_PATH'] = cudaToolkitPath
    env['ENV']['CUDA_INC_PATH'] = cudaToolkitPath + '/include'
    env['ENV']['CUDA_LIB_PATH'] = cudaToolkitPath + cudaLibDir

    env.Tool("cuda")

    release = env.get('RELEASE',None)
    if release == 'debug':
	env.AppendUnique(NVCCFLAGS=['-g'])
    if release == 'release':
	env.AppendUnique(NVCCFLAGS=['-O'])

    # add required libraries
    env.AppendUnique(CPPPATH=[cudaToolkitPath + '/include'])
    env.AppendUnique(LIBPATH=[cudaToolkitPath + cudaLibDir])
    env.AppendUnique(LIBS=['cudart'])

    if cudaSDKPath is not None:
	cudaSDKPath += '/C'
	env.AppendUnique(CPPPATH=[cudaSDKPath + '/common/inc'])
	env.AppendUnique(LIBPATH=[cudaSDKPath + cudaSDKLibDir, cudaSDKPath + '/common/'+cudaSDKLibDir + cudaSDKSubLibDir])
	if cutil:
	    if env['PLATFORM'] == 'win32':
		bits = GetArchBits(env)
		env.AppendUnique(LIBS=['cutil'+bits])
	    else:
		env.AppendUnique(LIBS=['cutil'])


# Helper functions

def FindCudaToolkit():
    home=os.environ.get('HOME', '')
    programfiles=os.environ.get('PROGRAMFILES', '')
    homedrive=os.environ.get('HOMEDRIVE', '')

    paths=[ home + '/NVIDIA_CUDA_TOOLKIT',
	    home + '/Apps/NVIDIA_CUDA_TOOLKIT',
	    home + '/Apps/NVIDIA_CUDA_TOOLKIT',
	    home + '/Apps/CudaToolkit',
	    home + '/Apps/CudaTK',
	    '/usr/local/NVIDIA_CUDA_TOOLKIT',
	    '/usr/local/CUDA_TOOLKIT',
	    '/usr/local/cuda_toolkit',
	    '/usr/local/CUDA',
	    '/usr/local/cuda',
	    '/Developer/NVIDIA CUDA TOOLKIT',
	    '/Developer/CUDA TOOLKIT',
	    '/Developer/CUDA',
	    '/opt/nvidia/cuda/toolkit',
	    '/opt/NVIDIA/CUDA/Toolkit',
	    '/opt/nvidia/cuda toolkit',
	    '/opt/NVIDIA/CUDA Toolkit',
	    programfiles + 'NVIDIA Corporation/NVIDIA CUDA TOOLKIT',
	    programfiles + 'NVIDIA Corporation/NVIDIA CUDA',
	    programfiles + 'NVIDIA Corporation/CUDA TOOLKIT',
	    programfiles + 'NVIDIA Corporation/CUDA',
	    programfiles + 'NVIDIA/NVIDIA CUDA TOOLKIT',
	    programfiles + 'NVIDIA/NVIDIA CUDA',
	    programfiles + 'NVIDIA/CUDA TOOLKIT',
	    programfiles + 'NVIDIA/CUDA',
	    programfiles + 'CUDA TOOLKIT',
	    programfiles + 'CUDA',
	    homedrive + '/CUDA TOOLKIT',
	    homedrive + '/CUDA']

    for path in paths:
	if os.path.isdir(path):
	    print 'scons: CUDA Toolkit found in ' + path
	    return path
    
    return None

def FindCudaSDK():
    home=os.environ.get('HOME', '')
    programfiles=os.environ.get('PROGRAMFILES', '')
    homedrive=os.environ.get('HOMEDRIVE', '')

    paths=[ home + '/NVIDIA_CUDA_SDK', # i am just guessing here
	    home + '/Apps/NVIDIA_CUDA_SDK',
	    home + '/Apps/CudaSDK',
	    '/usr/local/NVIDIA_CUDA_SDK',
	    '/usr/local/CUDASDK',
	    '/usr/local/cuda_sdk',
	    '/Developer/NVIDIA CUDA SDK',
	    '/Developer/CUDA SDK',
	    '/Developer/CUDA',
	    '/opt/nvidia/cuda/sdk',
	    '/opt/NVIDIA/CUDA/SDK',
	    '/opt/nvidia/cuda sdk',
	    '/opt/NVIDIA/CUDA SDK',
	    programfiles + 'NVIDIA Corporation/NVIDIA CUDA SDK',
	    programfiles + 'NVIDIA/NVIDIA CUDA SDK',
	    programfiles + 'NVIDIA CUDA SDK',
	    programfiles + 'CudaSDK',
	    homedrive + '/NVIDIA CUDA SDK',
	    homedrive + '/CUDA SDK',
	    homedrive + '/CUDA/SDK']
    for path in paths:
	if os.path.isdir(path):
	    print 'scons: CUDA SDK found in ' + path
	    return path

    return None

