General Info
============

LaTeX is able to produce really nice document layouts. But it is also able to
produce a lot of noise on the command line.  `pplatex` is a command-line tool
that parses the logs of latex and pdflatex and prints warnings and errors in an
human readable format.

`pplatex` will transform something like this
```
]
\openout2 = `chapter.aux'.


No file chapter.tex.
! Undefined control sequence.
l.9 Something \unknown
```
into this
```
** Warning in ./test.tex: No file chapter.tex.

** Error   in ./test.tex, Line 9:
   Undefined control sequence Something \unknown

Result: o) Errors: 1, Warnings: 1, BadBoxes: 0
```

The code is based on the LaTeX output parser of [Kile](http://kile.sourceforge.net/) (also used by TexMakerX),
with some modifications and bugfixes. Be aware that since the log output of the
LaTeX tools is not well defined (in any sense of the word), parsing is done by
a heuristic that tries its best but still might fail in some cases (e.g., having
very long directory names with spaces or special characters *might* cause issues).

In contrast to [rubber](https://launchpad.net/rubber), pplatex does *not* run your latex tools multiple times 
when references change or compile your images or the like. This remains the task
of you / your makefile. The git repository however does contain SCons extensions
to compile LaTeX documents, images and lots of other stuff using SCons (see the 
`SConstruct` file for an example).


Download
========

Source packages and precompiled binaries can be found on this project's releases page on Github.

  https://github.com/stefanhepp/pplatex/releases

For the Windows binaries to work, you will need to download and install the [Visual C++ Redistributable for Visual Studio 2015 (x86)](https://www.microsoft.com/en-us/download/details.aspx?id=48145) if you do not have it already.

Quick Start
===========

In your latex project directory, just run

    path/to/ppdflatex myfile.tex

Use pplatex instead to run latex instead of pdflatex.

If your latex tools are not in your PATH, use 

    path/to/pplatex -c path/to/pdflatex -- myfile.tex

You can also use pplatex to parse an existing log file…

    # run pdflatex normally
    pdflatex myfile.tex
    # Process the logfile and print warnings and errors.
    pplatex -i myfile.log

…or to filter the output of pdflatex/xelatex/lualatex:

    pdflatex myfile.tex | pplatex -i

Building
========

Requirements
------------

You will need a copy of PCRE (not PCRE2). Any version >= 7.0 should work fine.

  http://www.pcre.org/

On Ubuntu/Debian, you can get it with

    sudo apt-get install libpcre3-dev

On Windows, either download the source package and build it with cmake, or
download and unpack a precompiled binary package.

You can build either with SCons or CMake.

- SCons (>= 1.2.0)
  http://www.scons.org/
- CMake (>= 3.0)
  http://www.cmake.org/


Building with SCons
-------------------

If the pcre headers are not in a standard path (or if you are on Windows), 
create a './config.py' or './config-windows.py' (p.e. by copying if from 
./build/), and set PCREPATH to your PCRE installation path. 

If you are linking against a *static* version of PCRE on Windows (e.g., when you
did compile it yourself), then you need to uncomment the following line in
`SConstruct` (I have not tested this yet though!):

    env.Append( CPPDEFINES=['PCRE_STATIC'] )

To compile a release version of pplatex, simply use

    scons

For a debug version, use

    scons RELEASE=debug


Building with CMake
-------------------

If you are linking against a *dynamic* version of PCRE on Windows (e.g., when
you downloaded a precompiled package), comment out or remove this line in 
`src/CMakeLists.txt`

    add_definitions(-DPCRE_STATIC)

Create a build directory, ideally outside of your source checkout. Run either
`cmake` or `cmake-gui` inside the build directory and pass the source directory
as option the first time you run it.

    # Assuming your source directory is called 'pplatex'
    cd ..
    mkdir build
    cd build
    cmake-gui ../pplatex

On Linux, cmake without any further configuration should work fine. On Windows,
ensure that `CMAKE_BUILD_TYPE` is the same as your PCRE library was compiled
with (if you link statically), set `PCRE_INCLUDE_DIR` to the directory
containing `pcreposix.h`, and set `PCRE_PCRE_LIBRARY` and
`PCRE_PCREPOSIX_LIBRARY` to the full filename of `pcre.lib` and `pcreposix.lib`,
respectively. Run `Configure` and `Generate` and select nmake or Makefiles as
build type.

Run `make` (on Linux) or `nmake` on Windows in the build dir. This should
generate `pplatex.exe`. You can then copy or rename it to `ppdflatex.exe` or
`ppluatex.exe` and copy it to your install directory. Do not forget to copy
pcre.dll and pcreposix.dll into the same directory if you linked dynamically.


Installation
============

Place the files from the './bin' directory into any directory you like. Make
sure that that directory is in your PATH variable, or start the tools using the
installation path like

    /path/to/pplatex/ppdflatex <options>


Usage
=====

If the pdflatex and latex tools are in your PATH (try running 'latex' on your 
commandline), you can simply use ppdflatex if you want to run pdflatex, and pplatex 
for latex, like

    pplatex myfile.tex

or 

    ppdflatex myfile.tex

Warnings and badbox messages can be hidden like this

    pplatex -q -- latexfile.tex

In order to parse an existing log file, use

    pplatex -i somefile.log

To specify which latex program should be used (p.e. when latex is not in PATH), use the 
--cmd option, like

    pplatex -c /path/to/latex.exe -- <latex options> myfile

Make sure you do not use an interaction mode where latex waits for user input on 
errors. pplatex uses -interaction=nonstopmode by default if no interaction mode is 
specified.

The binaries are actually the same, pplatex detects the tool to run based on 
its filename. You can also call it ppluatex to run lualatex. Note that at the
moment, pplatex will default to lualatex if it is renamed to anything else. This
might change in the future.

On Linux, ppluatex is a script that calls pplatex. It allows configuration of
the location of pplatex and latex/pdflatex in `~/.ppluatex.conf`. Obviously, you
can also rename and use that script for pdflatex.


Open Tasks
==========

- [ ] Support warnings and error messages of PGF / TikZ
- [ ] Add option to SCons and CMake to choose whether to link PCRE statically or dynamically.
- [ ] Check for bugfixes in updates in Kile's parser and integrate them. Submit bugfixes in pplatex to Kile.

