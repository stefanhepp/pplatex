GENERAL INFO
------------

This is a small commandline wrapper tool for latex and pdflatex which prints 
prettier error messages than the latex tools.

For more informations, see

  http://www.stefant.org/web/projects/software/pplatex.html


QUICK START
-----------

In your latex project directory, run

# path/to/pplatex.exe myfile.tex

Use ppdflatex.exe instead to run pdflatex.

If your latex tools cannot be found automatically, use 

# path/to/pplatex.exe -c path/to/latex.exe -- myfile.tex


BUILDING
--------

To build, you will need

- SCons >= 1.2.0
  http://www.scons.org/
- PCRE
  http://www.pcre.org/

If the pcre headers are not in a standard path, create a './config.py' or 
'./config-windows.py' (p.e. by copying if from ./build/), and set PCREPATH to 
your PCRE installation path. On linux systems, install the 'libpcreposix3-dev' 
package.

To compile a release version of pplatex, simply use

# scons

For a debug version, use

# scons RELEASE=debug


INSTALLATION
------------

Place the files in the './bin' directory into any directory you like. Make sure that 
the installation directory is in your PATH variable, else you need to start the 
tools using the installation path like

# /path/to/pplatex/ppdflatex <options>


USAGE
-----

If the pdflatex and latex tools are in your PATH (try running 'latex' on your 
commandline), you can simply use ppdflatex if you want to run pdflatex, and pplatex 
for latex, like

# pplatex myfile.tex

or 

# ppdflatex myfile.tex

To specify which program should be used (p.e. if latex is not in PATH), use the 
--cmd option, like

# pplatex -c /path/to/latex.exe -- <latex options> myfile

Make sure you do not use an interaction mode where latex waits for user input on 
errors. pplatex uses -interaction=nonstopmode by default if no interaction mode is 
specified.

