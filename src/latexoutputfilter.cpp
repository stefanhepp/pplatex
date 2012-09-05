/************************************************************************************
    begin                : Die Sep 16 2003
    copyright            : (C) 2003 by Jeroen Wijnhout (wijnhout@science.uva.nl)
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

#include "latexoutputfilter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <pcreposix.h>

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "regex.h"

using namespace std;

////// Helper Functions //////

#define KILE_DEBUG() cerr

static bool file_exists(const string &name)
{
    struct stat info;
    int ret = stat(name.c_str(), &info);

    if ( ret == 0 ) {
	if ( info.st_mode & S_IFDIR ) {
	    return false;
	}
	return true;
    }

    return false;
}

static int parseInt(const string& str)
{
    std::istringstream iss(str);
    int i;
    iss >> i;
    return i;
}

static string trim(const string& str) {

    size_t start = str.find_first_not_of(" \t\n\r");

    if ( start == string::npos ) {
        return "";
    }

    size_t end   = str.find_last_not_of(" \t\n\r");

    return str.substr( start, end-start+1 );
}

static bool startsWith(const string &str, const string &match) {
    return str.compare(0, match.length(), match) == 0;
}

static bool startsWith(const string &str, char c) {
    return str.length() > 0 && str[0] == c;
}

static bool endsWith(const string &str, char c) {
    return str.length() > 0 && str[str.length()-1] == c;
}


////// Class Code //////

LatexOutputFilter::LatexOutputFilter(const string& source, const string& logfile, int verbose, bool nobadboxes, bool quiet) :
    OutputFilter(source, logfile, verbose),
    m_nErrors(0),
    m_nWarnings(0),
    m_nBadBoxes(0),
    m_nobadboxes(nobadboxes),
    m_quiet(quiet)
{
}

LatexOutputFilter::~ LatexOutputFilter()
{
}

bool LatexOutputFilter::fileExists(const string & name)
{
    bool isAbsolute = false;

    // TODO Quick hack to check if filename is an absolute path
    if (name.length() > 1 && (name[0] == '/' || name[1] == ':')) {
	isAbsolute = true;
    }

    if ( isAbsolute ) {
	return file_exists(name);
    }

    string file = "";
    if ( path() != "" && path() != "." ) {
	file = path() + '/';
    }
    file += name;

    if ( file_exists(file) ) {
	return true;
    }

    if ( file_exists(file + ".tex") ) {
	return true;
    }

    // try to determine the LaTeX source file
    /*
    QStringList extlist = m_extensions->latexDocuments().split(' ');
    for(QStringList::Iterator it = extlist.begin(); it!=extlist.end(); ++it) {
	    fi.setFile(path() + '/' + name + (*it));
	    if(fi.exists() && !fi.isDir()) {
		    return true;
	    }
    }
    */

    return false;
}

// There are basically two ways to detect the current file TeX is processing:
//	1) Use \Input (i.c.w. srctex.sty or srcltx.sty) and \include exclusively. This will
//	cause (La)TeX to print the line ":<+ filename"  in the log file when opening a file,
//	":<-" when closing a file. Filenames pushed on the stack in this mode are marked
//	as reliable.
//
//	2) Since people will probably also use the \input command, we also have to be
//	to detect the old-fashioned way. TeX prints '(filename' when opening a file and a ')'
//	when closing one. It is impossible to detect this with 100% certainty (TeX prints many messages
//	and even text (a context) from the TeX source file, there could be unbalanced parentheses),
//	so we use a heuristic algorithm. In heuristic mode a ')' will only be considered as a signal that
//	TeX is closing a file if the top of the stack is not marked as "reliable".
//	Also, when scanning for a TeX error linenumber (which sometimes causes a context to be printed
//	to the log-file), updateFileStack is not called, helping not to pick up unbalanced parentheses
//	from the context.
void LatexOutputFilter::updateFileStack(const string &strLine, short& dwCookie)
{
	//KILE_DEBUG() << "==LatexOutputFilter::updateFileStack()================" << endl;
	static string strPartialFileName;

	switch (dwCookie) {
		//we're looking for a filename
		case Start : case FileNameHeuristic :
			//TeX is opening a file
			if(startsWith(strLine,":<+ ")) {
// 				KILE_DEBUG() << "filename detected" << endl;
				//grab the filename, it might be a partial name (i.e. continued on the next line)
				strPartialFileName = trim(strLine.substr(4));

				//change the cookie so we remember we aren't sure the filename is complete
				dwCookie = FileName;
			}
			//TeX closed a file
			else if(startsWith(strLine,":<-")) {
// 				KILE_DEBUG() << "\tpopping : " << m_stackFile.top().file() << endl;
				if(!m_stackFile.empty()) {
					m_stackFile.pop();
				}
				dwCookie = Start;
			}
			else {
			//fallback to the heuristic detection of filenames
				updateFileStackHeuristic(strLine, dwCookie);
			}
		break;

		case FileName :
			//The partial filename was followed by '(', this means that TeX is signalling it is
			//opening the file. We are sure the filename is complete now. Don't call updateFileStackHeuristic
			//since we don't want the filename on the stack twice.
			if(startsWith(strLine,'(') || startsWith(strLine,"\\openout")) {
				//push the filename on the stack and mark it as 'reliable'
				m_stackFile.push(LOFStackItem(strPartialFileName, true));
// 				KILE_DEBUG() << "\tpushed : " << strPartialFileName << endl;
				strPartialFileName.clear();
				dwCookie = Start;
			}
			//The partial filename was followed by an TeX error, meaning the file doesn't exist.
			//Don't push it on the stack, instead try to detect the error.
			else if(startsWith(strLine,'!')) {
// 				KILE_DEBUG() << "oops!" << endl;
				dwCookie = Start;
				strPartialFileName.clear();
				detectError(strLine, dwCookie);
			}
			else if(startsWith(strLine,"No file")) {
// 				KILE_DEBUG() << "No file: " << strLine << endl;
				dwCookie = Start;
				strPartialFileName.clear();
				detectWarning(strLine, dwCookie);
			}
			//Partial filename still isn't complete.
			else {
// 				KILE_DEBUG() << "\tpartial file name, adding" << endl;
				strPartialFileName = strPartialFileName + trim(strLine);
			}
		break;

		default:
			break;
	}
}

void LatexOutputFilter::updateFileStackHeuristic(const string &strLine, short & dwCookie)
{
	//KILE_DEBUG() << "==LatexOutputFilter::updateFileStackHeuristic()================";
	static string strPartialFileName;
	bool expectFileName = (dwCookie == FileNameHeuristic);
	int index = 0;

	// handle special case (bug fix for 101810)
	if(expectFileName && strLine.length() > 0 && strLine[0] == ')') {
		m_stackFile.push(LOFStackItem(strPartialFileName));
		expectFileName = false;
		dwCookie = Start;
	}

	//scan for parentheses and grab filenames
	for(unsigned int i = 0; i < strLine.length(); ++i) {
		/*
		We're expecting a filename. If a filename really ends at this position one of the following must be true:
			1) Next character is a space (indicating the end of a filename (yes, there can't spaces in the
			path, this is a TeX limitation).
		comment by tbraun: there is a workround \include{{"file name"}} according to http://groups.google.com/group/comp.text.tex/browse_thread/thread/af873534f0644e4f/cd7e0cdb61a8b837?lnk=st&q=include+space+tex#cd7e0cdb61a8b837,
		but this is currently not supported by kile.
		comment by Stefan Hepp: the directory containing the tex-files *can* contain spaces!
			2) We're at the end of the line, the filename is probably continued on the next line.
			3) The TeX was closed already, signalled by the ')'.
		*/

		bool isLastChar = (i+1 == strLine.length());
		char nextIsTerminator;

		if (isLastChar) {
		    nextIsTerminator = 0;
		} else {
		    char c = strLine[i+1];
		    // TODO check if we are surrounded by " " (i.e. "c:\Documents and Settings\..", if so, wait for next "
		    nextIsTerminator = (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == ')') ? c : (char)0;
		}

		if(expectFileName && (isLastChar || nextIsTerminator)) {
			//KILE_DEBUG() << "Update the partial filename " << strPartialFileName << endl;
			strPartialFileName =  strPartialFileName + strLine.substr(index, i-index + 1);
			// we may continue for dirnames with spaces in it
			index = i + 1;

			if(strPartialFileName.empty()){ // nothing left to do here
			  continue;
			}

			//FIXME: improve these heuristics
			if((isLastChar && (i < 78)) || nextIsTerminator == ')' || nextIsTerminator == '\t' || fileExists(strPartialFileName)) {
				m_stackFile.push(LOFStackItem(strPartialFileName));
				// KILE_DEBUG() << "\tpushed (i = " << i << " length = " << strLine.length() << "): " << strPartialFileName << endl;
				expectFileName = false;
				dwCookie = Start;
			}
			//Guess the filename is continued on the next line, only if the current strPartialFileName does not exist, see bug # 162899
			else if(isLastChar || nextIsTerminator) {
				if(fileExists(strPartialFileName)) {
					m_stackFile.push(LOFStackItem(strPartialFileName));
					//KILE_DEBUG() << "pushed (i = " << i << " length = " << strLine.length() << "): " << strPartialFileName << endl;
					expectFileName = false;
					dwCookie = Start;
				}
				else {
					//KILE_DEBUG() << "Filename spans more than one line." << endl;
					dwCookie = FileNameHeuristic;
				}
			}
			//bail out
			else {
				dwCookie = Start;
				strPartialFileName.clear();
				expectFileName = false;
			}
		}
		//TeX is opening a file
		else if(strLine[i] == '(') {
			//we need to extract the filename
			expectFileName = true;
			strPartialFileName.clear();
			dwCookie = Start;

			//this is were the filename is supposed to start
			index = i + 1;
        	}
		//TeX is closing a file
		else if(strLine[i] == ')') {
			// KILE_DEBUG() << "\tpopping : " << m_stackFile.top().file() << endl;
			//If this filename was pushed on the stack by the reliable ":<+-" method, don't pop
			//a ":<-" will follow. This helps in preventing unbalanced ')' from popping filenames
			//from the stack too soon.
			if(m_stackFile.size() > 1 && !m_stackFile.top().reliable()) {
				m_stackFile.pop();
			}
			else {
				//KILE_DEBUG() << "\t\toh no, forget about it!";
			}
		}
	}
}


void LatexOutputFilter::flushCurrentItem()
{
    //KILE_DEBUG() << "==LatexOutputFilter::flushCurrentItem()================" << endl;
    int nItemType = m_currentItem.type();

    /* TODO why is this needed??
    while( m_stackFile.size() > 0 && !fileExists(m_stackFile.top().file()) ) {
	m_stackFile.pop();
    }
    */

    string sourceFile = (m_stackFile.empty()) ? source() : m_stackFile.top().file();
    m_currentItem.setSource(sourceFile);

    switch (nItemType) {
	case itmError:
	    ++m_nErrors;
	    break;

	case itmWarning:
	    ++m_nWarnings;
	    break;

	case itmBadBox:
	    ++m_nBadBoxes;
	    break;

	default: break;
    }

    // print message
    if ( nItemType == itmError || (nItemType == itmWarning && !m_quiet) || 
	(nItemType == itmBadBox && !m_nobadboxes) ) {
	cout << m_currentItem.getMessage();
    }

    m_currentItem.clear();
}

bool LatexOutputFilter::detectError(const string & strLine, short &dwCookie)
{
	//KILE_DEBUG() << "==LatexOutputFilter::detectError(" << strLine << ")================" << endl;

	bool found = false, flush = false;

	static Regex reLaTeXError("^(! )?(LaTeX|pdfTeX|Package|Class) ((.*) )?Error.*:(.*)", true);
	static Regex rePDFLaTeXError("^Error: pdflatex (.*)$", true);
	static Regex reTeXError("^! (.*)\\.$");
	static Regex reLineNumber("^l\\.([0-9]+)(.*)");

	switch (dwCookie) {
		case Start :
			if(reLaTeXError.match(strLine)) {
				//KILE_DEBUG() << "\tError : " <<  reLaTeXError.getMatch(strLine,1) << endl;
				m_currentItem.setMessage(reLaTeXError.getMatch(strLine,5));
				if (!reLaTeXError.getMatch(strLine,4).empty()) {
				    m_currentItem.setPackage(reLaTeXError.getMatch(strLine,2), reLaTeXError.getMatch(strLine,4));
				}
				found = true;
			}
			else if(rePDFLaTeXError.match(strLine)) {
				//KILE_DEBUG() << "\tError : " <<  rePDFLaTeXError.getMatch(strLine,1) << endl;
				m_currentItem.setMessage(rePDFLaTeXError.getMatch(strLine,1));
				found = true;
			}
			else if(reTeXError.match(strLine)) {
				//KILE_DEBUG() << "\tError : " <<  reTeXError.getMatch(strLine,1) << endl;
				m_currentItem.setMessage(reTeXError.getMatch(strLine,1));
				found = true;
			}
			if(found) {
				dwCookie = endsWith(strLine,'.') ? LineNumber : Error;
			}
		break;

		case Error :
			//KILE_DEBUG() << "\tError (cont'd): " << strLine << endl;
			if(endsWith(strLine,'.')) {
				dwCookie = LineNumber;
				m_currentItem.addMessage(strLine);
			}
			else if(GetCurrentOutputLine() - m_currentItem.outputLine() > 5) {
				cerr << "\tBAILING OUT: error description spans more than five lines" << endl;
				dwCookie = Start;
				flush = true;
			}
		break;

		case LineNumber :
			//KILE_DEBUG() << "\tLineNumber " << endl;
			if(reLineNumber.match(strLine)) {
				dwCookie = Start;
				flush = true;
				//KILE_DEBUG() << "\tline number: " << reLineNumber.getMatch(strLine,1) << endl;
				m_currentItem.setSourceLine(parseInt(reLineNumber.getMatch(strLine,1)));
				m_currentItem.addMessage(reLineNumber.getMatch(strLine,2));
			}
			else if(GetCurrentOutputLine() - m_currentItem.outputLine() > 10) {
				dwCookie = Start;
				flush = true;
				cerr << "\tBAILING OUT: did not detect a TeX line number for an error" << endl;
				m_currentItem.setSourceLine(0);
			} else {
				m_currentItem.addMessage(strLine);
			}
		break;

		default : break;
	}

	if(found) {
		m_currentItem.setType(itmError);
		m_currentItem.setOutputLine(GetCurrentOutputLine());
	}

	if(flush) {
		flushCurrentItem();
	}

	return found;
}

bool LatexOutputFilter::detectWarning(const string & strLine, short &dwCookie)
{
	//KILE_DEBUG() << "==LatexOutputFilter::detectWarning(" << strLine.length() << ")================" << endl;

// TODO: correctly match: (add package name to msg, remove '(<package>)      ' from following line
// Package hyperref Warning: Option `pdfpagelabels' is turned off
// (hyperref)                because \thepage is undefined.
//
// TODO match multiline, multiparagraph messages (?)

	bool found = false, flush = false;
	string warning;

	static Regex reLaTeXWarning("^(! )?(LaTeX|pdfTeX|Package|Class) ((.*) )?Warning.*:(.*)$", true);
	static Regex reNoFile("No file (.*)");
	// FIXME can be removed when http://sourceforge.net/tracker/index.php?func=detail&aid=1772022&group_id=120000&atid=685683 has promoted to the users
	static Regex reNoAsyFile("File .* does not exist.");

	switch(dwCookie) {
	    //detect the beginning of a warning
	    case Start :
		if(reLaTeXWarning.match(strLine)) {
		    warning = reLaTeXWarning.getMatch(strLine,5);
		    //KILE_DEBUG() << "\tWarning found: " << warning << endl;

		    found = true;
		    dwCookie = Start;

		    m_currentItem.setMessage(warning);
		    m_currentItem.setOutputLine(GetCurrentOutputLine());

		    if (!reLaTeXWarning.getMatch(strLine,4).empty()) {
			m_currentItem.setPackage(reLaTeXWarning.getMatch(strLine,2), reLaTeXWarning.getMatch(strLine,4));
		    }

		    //do we expect a line number?
		    flush = detectLaTeXLineNumber(warning, dwCookie, strLine.length());
		}
		else if(reNoFile.match(strLine)) {
		    found = true;
		    flush = true;
		    m_currentItem.setSourceLine(0);
		    m_currentItem.setMessage(reNoFile.getMatch(strLine,0));
		    m_currentItem.setOutputLine(GetCurrentOutputLine());
		}
		else if(reNoAsyFile.match(strLine)) {
		    found = true;
		    flush = true;
		    m_currentItem.setSourceLine(0);
		    m_currentItem.setMessage(reNoAsyFile.getMatch(strLine,0));
		    m_currentItem.setOutputLine(GetCurrentOutputLine());
		}

		break;

	    //warning spans multiple lines, detect the end
	    case Warning :
		flush = detectLaTeXLineNumber(warning, dwCookie, strLine.length());
		m_currentItem.addMessage(strLine);
		break;

	    default:
		break;
	}

	if(found) {
	    m_currentItem.setType(itmWarning);
	    m_currentItem.setOutputLine(GetCurrentOutputLine());
	}

	if(flush) {
	    flushCurrentItem();
	}

	return found;
}

bool LatexOutputFilter::detectLaTeXLineNumber(string & warning, short & dwCookie, size_t len)
{
	//KILE_DEBUG() << "==LatexOutputFilter::detectLaTeXLineNumber(" << warning.length() << ")================" << endl;

	static Regex reLaTeXLineNumber("(.*) on input line ([0-9]+)\\.$", true);
	static Regex reInternationalLaTeXLineNumber("(.*)([0-9]+)\\.$", true);

	if(reLaTeXLineNumber.match(warning)) {
		//KILE_DEBUG() << "een" << endl;
		m_currentItem.setSourceLine(parseInt(reLaTeXLineNumber.getMatch(warning,2)));
		warning = reLaTeXLineNumber.getMatch(warning,1);
		dwCookie = Start;
		return true;
	}
	else if(reInternationalLaTeXLineNumber.match(warning)) {
		//KILE_DEBUG() << "een" << endl;
		m_currentItem.setSourceLine(parseInt(reInternationalLaTeXLineNumber.getMatch(warning,2)));
		dwCookie = Start;
		return true;
	}
	else if(endsWith(warning,'.')) {
		//KILE_DEBUG() << "twee" << endl;
		m_currentItem.setSourceLine(-1);
		dwCookie = Start;
		return true;
	}
	//bailing out, did not find a line number
	else if((GetCurrentOutputLine() - m_currentItem.outputLine() > 8) || (len == 0)) {
		//KILE_DEBUG() << "drie current " << GetCurrentOutputLine() << " " <<  m_currentItem.outputLine() << " len " << len << endl;
		m_currentItem.setSourceLine(-1);
		dwCookie = Start;
		return true;
	}
	//error message is continued on the other line
	else {
		//KILE_DEBUG() << "vier" << endl;
		dwCookie = Warning;
		return false;
	}
}

bool LatexOutputFilter::detectBadBox(const string & strLine, short & dwCookie)
{
	//KILE_DEBUG() << "==LatexOutputFilter::detectBadBox(" << strLine.length() << ")================" << endl;

	bool found = false, flush = false;
	string badbox;

	static Regex reBadBox("^(Over|Under)(full \\\\[hv]box .*)", true);

	switch(dwCookie) {
	    case Start :
		if(reBadBox.match(strLine)) {
			found = true;
			dwCookie = Start;
			badbox = strLine;
			flush = detectBadBoxLineNumber(badbox, dwCookie, strLine.length());
			m_currentItem.setMessage(badbox);
		}
		break;

	    case BadBox :
		badbox = m_currentItem.message() + strLine;
		flush = detectBadBoxLineNumber(badbox, dwCookie, strLine.length());
		m_currentItem.setMessage(badbox);
		break;

	    default:
		    break;
	}

	if(found) {
	    m_currentItem.setType(itmBadBox);
	    m_currentItem.setOutputLine(GetCurrentOutputLine());
	}

	if(flush) {
	    flushCurrentItem();
	}

	return found;
}

bool LatexOutputFilter::detectBadBoxLineNumber(string & strLine, short & dwCookie, size_t len)
{
	//KILE_DEBUG() << "==LatexOutputFilter::detectBadBoxLineNumber(" << strLine.length() << ")================" << endl;

	static Regex reBadBoxLines("(.*) at lines ([0-9]+)--([0-9]+)", true);
	static Regex reBadBoxLine("(.*) at line ([0-9]+)", true);
	//Use the following only, if you know how to get the source line for it.
	// This is not simple, as TeX is not reporting it.
	static Regex reBadBoxOutput("(.*)has occurred while \\output is active^", true);

	string match = strLine;

	if(reBadBoxLines.match(strLine)) {
		dwCookie = Start;
		strLine = reBadBoxLines.getMatch(match,1);
		int n1 = parseInt(reBadBoxLines.getMatch(match,2));
		int n2 = parseInt(reBadBoxLines.getMatch(match,3));
		m_currentItem.setSourceLine(n1 < n2 ? n1 : n2);
		return true;
	}
	else if(reBadBoxLine.match(strLine)) {
		dwCookie = Start;
		strLine = reBadBoxLine.getMatch(match,1);
		m_currentItem.setSourceLine(parseInt(reBadBoxLine.getMatch(match,2)));
		//KILE_DEBUG() << "\tBadBox@" << reBadBoxLine.getMatch(strLine,2) << "." << endl;
		return true;
	}
	else if(reBadBoxOutput.match(strLine)) {
		dwCookie = Start;
		strLine = reBadBoxLines.getMatch(match,1);
		m_currentItem.setSourceLine(0);
		return true;
	}
	//bailing out, did not find a line number
	else if((GetCurrentOutputLine() - m_currentItem.outputLine() > 3) || (len == 0)) {
		dwCookie = Start;
		m_currentItem.setSourceLine(0);
		return true;
	}
	else {
		dwCookie = BadBox;
	}

	return false;
}

short LatexOutputFilter::parseLine(const string & strLine, short dwCookie)
{
	//KILE_DEBUG() << "==LatexOutputFilter::parseLine(" << strLine << dwCookie << strLine.length() << ")================" << endl;

	switch (dwCookie) {
		case Start :
			if(!(detectBadBox(strLine, dwCookie) || detectWarning(strLine, dwCookie) || detectError(strLine, dwCookie))) {
				updateFileStack(strLine, dwCookie);
			}
		break;

		case Warning :
			detectWarning(strLine, dwCookie);
		break;

		case Error : case LineNumber :
			detectError(strLine, dwCookie);
		break;

		case BadBox :
			detectBadBox(strLine, dwCookie);
		break;

		case FileName : case FileNameHeuristic :
			updateFileStack(strLine, dwCookie);
		break;

		default:
			dwCookie = Start;
			break;
	}

	return dwCookie;
}

bool LatexOutputFilter::run(FILE *out)
{
	m_nErrors = m_nWarnings = m_nBadBoxes = m_nParens = 0;
	while (!m_stackFile.empty()) {
	    m_stackFile.pop();
	}
	m_stackFile.push(LOFStackItem(source()));

	bool ret = OutputFilter::run(out);

	if ( m_currentItem.isValid() ) {
	    flushCurrentItem();
	}

	return ret;
}

/** Return number of errors etc. found in log-file. */
void LatexOutputFilter::getErrorCount(int *errors, int *warnings, int *badboxes)
{
    *errors = m_nErrors;
    *warnings = m_nWarnings;
    *badboxes = m_nBadBoxes;
}
