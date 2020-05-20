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
#include "inimanager.h"
#include "inisection.h"
#include <sstream>
#include "log.h"

using namespace std;


iniManager::iniManager()
{
}


iniManager::~iniManager()
{

	DeleteSections();
}

void iniManager::DeleteSections()
{
    for(map<string,iniSection*>::iterator it= m_mSections.begin(); it != m_mSections.end(); ++it)
	{
		delete it->second;
	}
	m_mSections.clear();
}

/*!
    \fn iniManager::ReadIniFile(const string& sFilename)
 */
bool iniManager::ReadIniFile(const string& sFilename)
{
	//unsigned int errno;
	DeleteSections();

    //Close file if open
	if(m_if.is_open())
		m_if.close();

	//attempt to open the file
	m_if.open(sFilename.c_str(),ios::in);
	if(!m_if.is_open())
	{
	    pml::Log::Get(pml::Log::LOG_ERROR) << "IniManager file '" << sFilename << "' does not exist" << endl;
		return false;
	}

	m_if.clear();

	string sLine;

	//read in each line
	while(!m_if.eof())
	{
		// if the line starts with a [ then its the start of a section
		if(sLine[0] == '[')
		{
			//get the name of the section
			size_t nClosePos = sLine.find("]");
			if(nClosePos == std::string::npos)	//this is an error
            {
                pml::Log::Get(pml::Log::LOG_ERROR) << "IniManager file '" << sFilename << "' invalid section" << endl;
                return false;
            }
			string sSection(sLine.substr(1,nClosePos-1));
			m_mSections[sSection] = new iniSection(sSection);
			sLine = m_mSections[sSection]->ReadSection(&m_if);
		}
		else
		{
			getline(m_if,sLine,'\n');
			//if the line starts with a # we ignore
			if(sLine[0] == '#')
				continue;
		}
	}
		//Close the file again
	if(m_if.is_open())
		m_if.close();
    return true;
}


/*!
    \fn iniManager::GetSectionBegin()
 */
std::map<string,iniSection*>::iterator iniManager::GetSectionBegin()
{
    return m_mSections.begin();
}

/*!
    \fn iniManager::GetSectionEnd()
 */
std::map<string,iniSection*>::iterator iniManager::GetSectionEnd()
{
    return m_mSections.end();
}

iniSection* iniManager::GetSection(const string& sSectionName)
{
    std::map<string,iniSection*>::iterator it = m_mSections.find(sSectionName);
    if(it == m_mSections.end())
        return NULL;
    return it->second;
}

/*!
    \fn iniManager::GetIniString(const string& sSection, const string& sKey, const string& sDefault)
 */
const string& iniManager::GetIniString(const string& sSection, const string& sKey, const string& sDefault)
{
    //does the section exist?
    itSection it = m_mSections.find(sSection);
	if(it==m_mSections.end())
		return sDefault;


	return it->second->GetString(sKey,sDefault);
}

/*!
    \fn iniManager::GetIniInt(const string& sSection, const string& sKey, int nDefault)
 */
int iniManager::GetIniInt(const string& sSection, const string& sKey, int nDefault)
{
    //does the section exist?
	itSection it = m_mSections.find(sSection);
	if(it==m_mSections.end())
		return nDefault;

	return it->second->GetInt(sKey,nDefault);
}

/*!
    \fn iniManager::GetIniDouble(const string& sSection, const string& sKey, double dDefault)
 */
double iniManager::GetIniDouble(const string& sSection, const string& sKey, double dDefault)
{
    //does the section exist?
	itSection it = m_mSections.find(sSection);
	if(it==m_mSections.end())
		return dDefault;

	return it->second->GetDouble(sKey,dDefault);
}

bool iniManager::WriteIniFile(const string& sFilename)
{
	//unsigned int errno;

    //Close file if open
	if(m_of.is_open())
		m_of.close();

	//attempt to open the file

	m_of.open(sFilename.c_str());
	if(!m_of.is_open())
	{
		return false;
	}

	m_of.clear();

	for(std::map<string,iniSection*>::iterator it = m_mSections.begin(); it != m_mSections.end(); ++it)
	{
		it->second->WriteSection(&m_of);
	}

		//Close the file again
	if(m_of.is_open())
		m_of.close();

    return true;

}

iniSection* iniManager::CreateSection(const string& sSectionName)
{
    map<string, iniSection*>::iterator itSection =m_mSections.find(sSectionName);
    if(itSection == m_mSections.end())
    {
        itSection  = m_mSections.insert(make_pair(sSectionName,new iniSection(sSectionName))).first;
    }
    return itSection->second;
}

void iniManager::SetSectionValue(const string& sSectionName, const string& sKey, const string& sValue)
{
    iniSection* pSection = CreateSection(sSectionName);
    pSection->SetValue(sKey,sValue);
}

void iniManager::SetSectionValue(const string& sSectionName, const string& sKey, int nValue)
{
    stringstream ss;
    ss << nValue;
    SetSectionValue(sSectionName, sKey, ss.str());

}

size_t iniManager::GetNumberOfSectionEntries(const string& sSectionName)
{
	itSection it = m_mSections.find(sSectionName);
	if(it== m_mSections.end())
        return 0;
    return it->second->GetNumberOfEntries();
}

bool iniManager::DeleteSection(const std::string& sSectionName)
{
    iniSection* pSection = GetSection(sSectionName);
    if(pSection)
    {
        delete pSection;
        m_mSections.erase(sSectionName);
        return true;
    }
    return false;
}
