/**********************************************************************************
    begin                : Die Sep 16 2003
    copyright            : (C) 2003 by Jeroen Wijnhout (wijnhout@science.uva.nl)
                               2008 by Michel Ludwig (michel.ludwig@kdemail.net)
			       2009 by Stefan Hepp (stefan@stefant.org)

    This file is taken from the Kile project and modified to work without Qt on
    the cmdline by Stefan Hepp (stefan@stefant.org).
 **********************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OUTPUTINFO_H
#define OUTPUTINFO_H

#include <string>
#include <stack>

/**
 * Class for output-information of third program (e.g. Latex-Output, C-Compiler output)
 *
 * @author Thorsten Lck
 * @author Jeroen Wijnhout
 * @author Stefan Hepp
 **/

class OutputInfo
{
    public:
	/**
	 * Constructs an invalid output information object.
	 **/
	OutputInfo();

	OutputInfo(const std::string &strSrcFile, int nSrcLine, int nOutputLine, const std::string &strError = "", int nErrorID = -1);

	/**
	 * Returns true if and only if this object contains valid output
	 * information.
	 **/
	bool isValid() const;

	/** Source file where error occurred. */
	std::string source() const { return m_strSrcFile; }
	/** Source file where error occurred. */
	void setSource(const std::string& src) { m_strSrcFile = src; }

	/** Line number in source file of the current message */
	int sourceLine() const { return m_nSrcLine; }
	/** Line number in source file of the current message */
	void setSourceLine(int line) { m_nSrcLine =  line; }

	/** Error message */
	const std::string message() const { return m_strError; }

	/** Error message */
	void setMessage(const std::string& message) { m_strError = message; }

	void setPackage(const std::string& msgClass, const std::string& package) { m_msgClass = msgClass; m_package = package; }

	const std::string getClass() const { return m_msgClass; }

	/** Error code */
	int type() const { return m_nErrorID; }
	/** Error code */
	void setType(int type) { m_nErrorID = type; }

	/** Line number in the output, where error was reported. */
	int outputLine() const { return m_nOutputLine; }
	/** Line number in the output, where error was reported. */
	void setOutputLine(int line) { m_nOutputLine = line; }

	/**
	 * Clears the information stored in this object, turning it
	 * into an invalid output information object.
	 **/
	void clear();

	/**
	 * Comparison operator
	 **/
	bool operator==(const OutputInfo& info) const;

    protected:
	std::string m_strSrcFile;
	int m_nSrcLine;
	std::string m_strError;
	std::string m_msgClass;
	std::string m_package;
	int m_nOutputLine;
	int m_nErrorID;
};

/**
 * Array of OutputInfo
 * @author Thorsten Lck
 **/
typedef std::stack<OutputInfo> OutputInfoArray;

class LatexOutputInfo : public OutputInfo
{
    public:
	LatexOutputInfo();
	LatexOutputInfo(const std::string& strSrcFile, int nSrcLine, int nOutputLine, const std::string& strError = "", int nErrorID = -1);
	
	std::string getMessage();

	void addMessage(const std::string& msg);

    public:
	/**
	 * These constants are describing, which item types is currently
	 * parsed. (to be set as error code)
	 **/
	enum tagCookies
	{
		itmNone = 0,
		itmError,
		itmWarning,
		itmBadBox
	};
};

/**
 * Array of LatexOutputInfo
 * @author Thorsten Lck
 **/
typedef std::stack<LatexOutputInfo> LatexOutputInfoArray;

#endif
