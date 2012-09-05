/**
 * @project pplatex
 * @author  Stefan Hepp (stefan@stefant.org)
 * 
 * Copyright: 2009 Stefan hepp
 * Licence: GPL v3
 * See 'COPYRIGHT.txt' for copyright and licensing information.
 **/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

#if defined( __WIN32__ ) || defined( _WIN32 )

#include <stdlib.h>
#define popen(x,y) _popen(x,y)
#define pclose(x) _pclose(x)
#define WEXITSTATUS(x) ((x) == -1 ? 1 : (x))

#else

#include <sys/wait.h>

#endif

#include "latexoutputfilter.h"

using namespace std;

class ArgParser {
    public:
	ArgParser(int argc, char** argv) 
	{
	    this->argc = argc;
	    this->argv = argv;
	    verbose = 0;
	    help = false;
	    version = false;
	    quiet = false;
	    nobadboxes = false;
	    
	    parseArguments();
	}

	int isVerbose() {
	    return verbose;
	}

	bool showUsage() {
	    return help;
	}

	bool showVersion() {
	    return version;
	}

	string getProgramName() {
	    return program;
	}

	string getSourcefile() {
	    return texfile;
	}

	string getLogfile() {
	    return logfile;
	}

	string getCmdline() {
	    return cmd;
	}

	bool doParseLogfile() {
	    return parseLogfile;
	}

	bool isQuiet() {
	    return quiet;
	}

	bool noBadBoxes() {
	    return nobadboxes;
	}
	
    private:
	int argc;
	char** argv;

	string logfile;
	string texfile;
	string program;
	string cmd;
	bool parseLogfile;
	int verbose;
	bool help;
	bool version;
	bool quiet;
	bool nobadboxes;

	void parseArguments() {

	    int options = 1;
	    int cmdopt = 0;
	    int verboseopt = 0;
	    int hasInteraction = 0;
	    int inputopt = 0;
	    int quietopt = 0;
	    int bbopt = 0;

	    string name(argv[0]);

	    if ( name.compare(name.length()-7,7,"pplatex") == 0 ) {
		program = "latex";
	    } else if ( name.compare(name.length()-9,9,"ppdflatex") == 0 ) {
		program = "pdflatex";
            } else {
		program = "lualatex";
	    }

	    char** args = &argv[1];

	    for (int i = 1; i < argc; i++, args++) {
		string arg(*args);
		if ( !cmdopt && (arg == "--cmd" || arg == "-c") ) {
		    cmdopt = i+1;
		}
		else if ( options == 1 && arg == "--" ) {
		    options = i+1;
		}
		else if ( !verboseopt && arg == "-v" ) {
		    verboseopt = i;
		}
		else if ( arg.compare(0, 13, "-interaction=") == 0 ) {
		    hasInteraction = i;
		}
		else if ( options == 1 && (arg == "-h" || arg == "--help") ) {
		    help = true;
		} 
		else if ( options == 1 && (arg == "-V" || arg == "--verbose") ) {
		    version = true;
		}
		else if ( !inputopt && (arg == "-i" || arg == "--input" ) ) {
		    inputopt = i+1;
		}
		else if ( !quietopt && (arg == "-q" || arg == "--quiet" ) ) {
		    quietopt = i;
		}
		else if ( !quietopt && (arg == "-b" || arg == "--nobadboxes" ) ) {
		    bbopt = i;
		}
	    }
	    
	    if (help || version) {
		return;
	    }

	    if ( inputopt ) {
		if ( options == 1 ) {
		    // accept all options as pplatex options if --input is used
		    options = argc + 1;
		} else if ( inputopt <= options || cmdopt ) {
		    cerr << "Option '--input' cannot be combined with latex options" << endl;
		    exit(2);
		}
	    }

	    if ( cmdopt && cmdopt < options && cmdopt < argc ) {
		program = argv[cmdopt];
	    }

	    if ( verboseopt && verboseopt < options ) {
		verbose = 1;
	    }

	    if ( quietopt && quietopt < options ) {
		quiet = true;
	    }
	    if ( bbopt && bbopt < options ) {
		nobadboxes = true;
	    }

	    if ( inputopt && inputopt < options ) {
		parseLogfile = true;
		logfile = inputopt < argc ? argv[inputopt] : "-";
	    } 
	    else 
	    {
		parseLogfile = false;

		// build cmdline
		if ( !buildCmd(argc, argv, options, hasInteraction) ) {
		    return;
		}
	
		// get logfile name
		
		size_t fpos = texfile.find_last_of("/\\");
		if ( fpos == string::npos ) {
		    fpos = 0;
		} else {
		    fpos++;
		}

		size_t epos = texfile.length();

		if (epos > 4) {
		    string ext = texfile.substr(texfile.length()-4);
		    if ( ext == ".tex" || ext == ".TEX" ) {
			epos -= 4;
		    }
		}

		logfile = texfile.substr( fpos, epos - fpos );
	    }
	}

	int buildCmd(int argc, char** argv, int options, int hasInteraction) {

	    cmd = program;

	    if ( hasInteraction && hasInteraction < options ) {
		cerr << "Invalid option for " << program << ": -interaction" << endl;
		exit(2);
	    }
	    if ( !hasInteraction ) {
		cmd += " -interaction=nonstopmode";
	    }
	    
	    if ( options >= argc ) {
		// no options, this is odd
		cerr << "No options given for '" << program << "', exiting!" << endl;	
		exit(2);
	    }

	    char** args = &argv[options];
	    int texopt = 0;

	    for (int i = options; i < argc; i++, args++) {
		cmd += ' ';
		cmd += *args;

		if ( !texopt && *args[0] != '-' ) {
		    texopt = i;
		    texfile = *args;
		}
	    }

	    if ( !texopt ) {
		if ( help ) {
		    return 0;
		}
		cerr << "No tex-file has been given!" << endl;
		exit(3);
	    }

	    return texopt;
	}
};

static void usage(char* program) {
    cout << "Usage: " << program << " [<pplatex options> --] <latex options>" << endl;
    cout << endl;
    cout << "  pplatex options:" << endl;
    cout << "    -c, --cmd <cmd>    Execute <cmd> to compile the tex file" << endl;
    cout << "    -i, --input <file> Parse logfile <file> instead of executing latex ('-' for stdin)" << endl;
    cout << "    -b                 Do not show badbox messages" << endl;
    cout << "    -q                 Do not show warnings and badbox messages" << endl;
    cout << "    -v                 Be verbosive" << endl;
    cout << "    -V, --version      Show version info" << endl;
    cout << "    -h, --help         Show this help" << endl;
    cout << endl;
    cout << "  By default, if the program is called 'pplatex', 'latex' will be executed," << endl;
    cout << "  else 'pdflatex' will be used." << endl;
}

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

static void version() {
    cout << STRINGIFY(RELEASE_NAME) << ": " << STRINGIFY(RELEASE_VERSION) << endl;
    cout << "Copyright (C) 2003 Jeroen Wijnhout (wijnhout@science.uva.nl)" << endl;
    cout << "              2008 by Michel Ludwig (michel.ludwig@kdemail.net)" << endl;
    cout << "              2009,2010 Stefan Hepp (stefan@stefant.org)" << endl;

}

int main(int argc, char** argv) {

    if ( argc == 1 ) {
	cout << "No arguments supplied. Try '" << argv[0] << " -h'." << endl;
	return 1;
    }

    ArgParser parser(argc, argv);

    if ( parser.showUsage() ) {
	usage(argv[0]);
	return 0;
    }

    if ( parser.showVersion() ) {
	version();
	return 0;
    }

    FILE *fp;
    
    if ( parser.doParseLogfile() ) {
	if ( parser.isVerbose() ) {
	    cout << "Parsing ";
	    if ( parser.getLogfile() == "-" ) {
		cout << "logfile " << parser.getLogfile() << endl;
	    } else {
		cout << "stdin" << endl;
	    }
	}

	if ( parser.getLogfile() == "-" ) {
	    fp = stdin;
	} else {
#ifdef _WIN32
	    errno_t err = fopen_s(&fp, parser.getLogfile().c_str(), "r");
	    if (err != 0) {
		fp = 0;
	    }
#else
	    fp = fopen(parser.getLogfile().c_str(), "r");
#endif
	}

	if ( !fp ) {
	    perror("Error");
	    cerr << "Unable to open "; 
	    if ( parser.getLogfile() == "-" ) {
		cout << "stdin" << endl;
	    } else {
		cout << "logfile " << parser.getLogfile() << endl;
	    }
	    return 1;
	}
    } else {
	if ( parser.isVerbose() ) {
	    cout << "Executing: " << parser.getCmdline() << endl;
	}

	fp = popen(parser.getCmdline().c_str(), "r");

	if ( !fp ) {
	    perror("Error");
	    cerr << "Unable to execute '" << parser.getProgramName() << "'!" << endl;
	    return 1;
	}
    }

    LatexOutputFilter of(parser.getSourcefile(), parser.getLogfile(), parser.isVerbose(), parser.noBadBoxes() || parser.isQuiet(), parser.isQuiet());

    of.run(fp);

    int errors, warnings, badboxes;
    of.getErrorCount( &errors, &warnings, &badboxes );

    cout << "Result: o) Errors: " << errors << ", Warnings: " << warnings << ", BadBoxes: " << badboxes << endl;

    int status;

    if ( parser.doParseLogfile() ) {
	fclose(fp);
	status = (errors > 0) ? 1 : 0;
	cout << endl;
    } else {
        int ret = pclose(fp);
	status = WEXITSTATUS(ret);

	if ( status ) {
	    cout << "        o) " << parser.getProgramName() << " returned an error!" << endl << endl;
	} else {
	    cout << "        o) " << parser.getProgramName() << " was successful!" << endl << endl;
	}
    }

    return status;
}

