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

#ifndef OUTPUTFILTER_H
#define OUTPUTFILTER_H

#include <string>

/**
 * An object of this class is used to parse the output messages 
 * of any third-party tool.
 *
 * @author Thorsten Lck
 * @author Jeroen Wijnhout
 * @author Stefan Hepp
 */

class OutputFilter 
{
    public:
	OutputFilter(const std::string& src, const std::string& log, int verbose);
	virtual ~OutputFilter();

	virtual bool run(FILE *out);

	const std::string& log() const { return m_log; }

	const std::string& source() const  { return m_source; }
	const std::string& path() const { return m_srcPath; }

    protected:
	virtual short parseLine(const std::string& strLine, short dwCookie);
	/**
	Returns the zero based index of the currently parsed line in the
	output file.
	*/
	int GetCurrentOutputLine() const;

    private:
	/** Number of current line in output file */
	unsigned int	m_nOutputLines;
	std::string	m_log, m_source, m_srcPath;
	int		m_verbose;
};
#endif
