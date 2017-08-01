/***************************************************************************
    begin                : Die Sep 16 2003
    copyright            : (C) 2003 by Jeroen Wijnhout
                               2009 by Stefan Hepp (stefan@stefant.org)
    email                : wijnhout@science.uva.nl

    This file is taken from the Kile project and modified to work without Qt on
    the cmdline by Stefan Hepp (stefan@stefant.org).
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "outputfilter.h"

#include <string>
#include <iostream>
#include <cstdio>

using namespace std;

static string trim(const string& str) {

    size_t start = str.find_first_not_of(" \t\n\r");

    if ( start == string::npos ) {
	return "";
    }
   
    // do not trim spaces at the end
    size_t end   = str.find_last_not_of("\t\n\r");

    return str.substr( start, end-start+1 );
}

OutputFilter::OutputFilter(const string& src, const string& log, int verbose) :
	m_log(log), m_source(src), m_verbose(verbose)
{
    // TODO maybe use better method to handle also escaped chars in filename

    size_t pos = src.find_last_of("/\\");
    if ( pos == string::npos ) {
	m_srcPath = ".";
    } else {
	m_srcPath = src.substr(0,pos);
    }
}

OutputFilter::~ OutputFilter()
{
}

short OutputFilter::parseLine(const string & /*strLine*/, short /*dwCookie*/)
{
    return 0;
}

bool OutputFilter::run(FILE* out)
{
    short sCookie = 0;
    string s = "";

    m_nOutputLines = 0;

    char line[120];
    line[119] = line[118] = 0;

    while ( fgets(line, sizeof(line), out )) {

	s += line;

	if ( line[118] != 0 && line[118] != '\n' ) {
	    // line is too long, continue reading
	    line[118] = 0;
	    continue;
	}

	if ( m_verbose ) {
	    cerr << s;
	}
	
	sCookie = parseLine(trim(s), sCookie);
	++m_nOutputLines;

	s.clear();
    }

    if ( ferror(out) ) {
	perror("Parsing stdout");
	return false;
    }

    return true;
}


int OutputFilter::GetCurrentOutputLine() const
{
    return m_nOutputLines;
}

