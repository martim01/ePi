/***************************************************************************
 *   Copyright (C) 2005 by Matthew Martin   *
 *   matthew@nabiezzi.plus.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "inisection.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "log.h"

using namespace std;

iniSection::iniSection(const string& sSection)
: m_sSectionName(sSection)
{

}


iniSection::~iniSection()
{
}




/*!
    \fn iniSection::ReadSection(std::ifstream* pif)
	reads lines from ifstream* pif until the end of the ini section or end of file
	returns the last line it read
 */
string iniSection::ReadSection(std::ifstream* pif)
{
   //we loop through the ini file until we hit the end, or a new section
	string sLine;
	while(!pif->eof())
	{
		getline(*pif,sLine,'\n');


		if(sLine[0] == '#' || sLine.size() < 2 || sLine[0] == ';')	//comment
			continue;

		if(sLine[0] == '[')	// new section
			return sLine;

		size_t nEqualPos = sLine.find("=");
		if(nEqualPos == std::string::npos)
		{
			pml::Log::Get(pml::Log::LOG_ERROR) << "line: "	<< sLine	<< endl;
			return "";
		}

		size_t nCommentPos = sLine.find("#",nEqualPos);
		if(nCommentPos == std::string::npos)
			nCommentPos = sLine.size();

		string sKey = sLine.substr(0,nEqualPos);
		m_mSectionData[sKey] = sLine.substr(nEqualPos+1, nCommentPos-(nEqualPos+1));
	}
	return sLine;
    //return NULL_STRING;
}


/*!
    \fn iniSection::GetDataBegin()
 */
std::map<string,string>::const_iterator iniSection::GetDataBegin() const
{
    return m_mSectionData.begin();
}

/*!
    \fn iniSection::GetDataEnd()
 */
std::map<string,string>::const_iterator iniSection::GetDataEnd() const
{
    return m_mSectionData.end();
}


/*!
    \fn iniSection::GetString(const string& sKey, const string& sDefault)
 */
const string& iniSection::GetString(const string& sKey, const string& sDefault)
{
  	//does the key exist
  	itIniData it = m_mSectionData.find(sKey);
	if(it==m_mSectionData.end())
		return sDefault;
	return it->second;
}

/*!
    \fn iniSection::GetInt(const string& sKey, int nDefault)
 */
int iniSection::GetInt(const string& sKey, int nDefault)
{
  	//does the key exist
  	itIniData it = m_mSectionData.find(sKey);
	if(it==m_mSectionData.end())
		return nDefault;
    long n = nDefault;

    n = atoi(it->second.c_str());
	return n;
}

/*!
    \fn iniSection::GetString(const string& sKey, double dDefault)
 */
double iniSection::GetDouble(const string& sKey, double dDefault)
{
  	//does the key exist
	itIniData it = m_mSectionData.find(sKey);
	if(it==m_mSectionData.end())
		return dDefault;
    double d = dDefault;
    d = atof(it->second.c_str());
    return d;
}

void iniSection::WriteSection(std::ofstream* pof)
{
	//write the section name
	*pof	<<	"["
			<<	m_sSectionName
			<<	"]\n";
	//now write the data
	std::map<string,string>::iterator it = m_mSectionData.begin();
	while(it != m_mSectionData.end())
	{
		*pof	<<	it->first
				<<	"="
				<<	it->second
				<<	"\n";
		++it;
	}
}

void iniSection::SetValue(const string& sKey, const string& sValue)
{
	m_mSectionData[sKey] = sValue;
}

size_t iniSection::GetNumberOfEntries()
{
    return m_mSectionData.size();
}
