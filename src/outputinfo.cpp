/************************************************************************************
    begin                : Die Sep 16 2003
    copyright            : (C) 2003 by Jeroen Wijnhout (wijnhout@science.uva.nl)
                               2008 by Michel Ludwig (michel.ludwig@kdemail.net)
			       2009 by Stefan Hepp (stefan@stefant.org)

    This file is taken from the Kile project and modified to work without Qt on
    the cmdline by Stefan Hepp (stefan@stefant.org).
 ************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "outputinfo.h"

#include <string>
#include <sstream>

#include "regex.h"

using namespace std;

OutputInfo::OutputInfo()
{
    clear();
}

OutputInfo::OutputInfo(const string& strSrcFile, int nSrcLine, int nOutputLine,
                       const string& strError, int nErrorID) :
    m_strSrcFile(strSrcFile),
    m_nSrcLine(nSrcLine),
    m_strError(strError),
    m_nOutputLine(nOutputLine),
    m_nErrorID(nErrorID)
{
}

void OutputInfo::clear()
{
    m_strSrcFile.clear();
    m_nSrcLine = -1;
    m_nOutputLine = -1;
    m_strError.clear();
    m_nErrorID = -1;
    m_msgClass.clear();
    m_package.clear();
}

bool OutputInfo::operator==(const OutputInfo& info) const
{
    return (m_strSrcFile == info.m_strSrcFile
	 && m_nSrcLine == info.m_nSrcLine
	 && m_strError == info.m_strError
	 && m_nOutputLine == info.m_nOutputLine
	 && m_nErrorID == info.m_nErrorID
	 && m_msgClass == info.m_msgClass
	 && m_package == info.m_package);
}

bool OutputInfo::isValid() const
{
    return !(m_strSrcFile.empty() && m_nSrcLine == -1 && m_nOutputLine == -1
				  && m_strError.empty() && m_nErrorID == -1);
}


LatexOutputInfo::LatexOutputInfo() : OutputInfo()
{
}


LatexOutputInfo::LatexOutputInfo(const string& strSrcFile, int nSrcLine, int nOutputLine,
                                 const string& strError, int nErrorID)
: OutputInfo(strSrcFile, nSrcLine, nOutputLine, strError, nErrorID)
{
}

void LatexOutputInfo::addMessage(const string& msg) 
{
    if (msg.empty() || 
	msg == "Type  H <return>  for immediate help." ||
	msg == "...") 
    {
	return;
    }

    const string regex = "^(\\(" + m_package + "\\))? *(.*)$";

    Regex regLine(regex.c_str());
    
    string line;

    if ( regLine.match(msg) ) {
	line = regLine.getMatch(msg, 2);
    } else {
	line = msg;
    }

    if (m_strError.length() + line.length() < 80) {
	m_strError = m_strError + line;
    } else {
	m_strError = m_strError + "\n   " + line;
    }
}

string LatexOutputInfo::getMessage()
{
    ostringstream msg;
    bool hasType = true;
    
    switch (m_nErrorID) {
	case itmError:   msg << "** Error  ";   break;
	case itmWarning: msg << "** Warning"; break;
	case itmBadBox:  msg << "** BadBox ";  break;
	default: hasType = false;
    }

    if ( !m_strSrcFile.empty() ) {
	if ( hasType ) {
	    msg << " in ";
	}
	msg << m_strSrcFile;
    }

    if ( m_nSrcLine > 0 ) {
	msg << ", Line " << m_nSrcLine;
    }

    msg << ": ";

    if (msg.str().length() + m_msgClass.length() + m_package.length() + m_strError.length() > 78 && !m_strError.empty() ) {
	msg << endl << "   ";
    }

    if (!m_msgClass.empty() ) {
	msg << "(" << m_msgClass << " " << m_package << ") ";
    }

    msg << m_strError << endl;

    msg << endl;

    return msg.str();
}

