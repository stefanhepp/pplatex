/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __PPLATEX_REGEX_H__
#define __PPLATEX_REGEX_H__

#include <pcreposix.h>
#include <string>

using namespace std;

class Regex 
{
    public:
	Regex(const char* regex, bool caseinsensitive = false);

	~Regex();

	bool match(const string& str);

	string getMatch(const string& str, int i);

    private:
	regex_t rx;
	regmatch_t *matches;

};

#endif
