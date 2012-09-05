/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "regex.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <pcreposix.h>

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

Regex::Regex(const char* regex, bool caseinsensitive)
    : matches(0)
{
    int ret = regcomp(&rx, regex, caseinsensitive ? REG_ICASE : 0 );

    if ( ret ) {
	char err[1024];
	regerror( ret, &rx, err, sizeof(err) );
	cerr << "Error parsing regex " << regex << ", " << err << endl;
	exit(3);
    }

    matches = (regmatch_t*) malloc( (rx.re_nsub+1) * sizeof(regmatch_t) );
}

Regex::~Regex() {
    // regfree( &rx );
    if ( matches ) free( matches );
}

bool Regex::match(const string& str)
{
    int ret = regexec( &rx, str.c_str(), rx.re_nsub+1, matches, 0 );
    if ( ret == 0 ) {
	return true;
    }
    if ( ret == REG_NOMATCH ) {
	return false;
    }

    char err[1024];
    regerror( ret, &rx, err, sizeof(err) );
    cerr << "Error matching string '" << str << "', " << err << endl;

    return false;
}

string Regex::getMatch(const string& str, int i) {
    if (matches == NULL)
        return str;
    else if ( matches[i].rm_so == -1 )
        return "";
    else
        return str.substr(matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so);
}

