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
#pragma once

#include <map>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>


class iniSection;


/**
*   @brief simple class to read and write ini files
*   @author Matthew Martin
*   @version 0.1
*   @date 2008
*
*   iniManager reads in an ini file and stores the sections, keys and values
*
**/
class iniManager{
public:
    /** @brief Constructor
    **/
    iniManager();

    /** @brief Destructor
    **/
    ~iniManager();

    /** @brief Reads in an inifile
    *   @param sFilename the path and filename of the ini file to read in
    *   @return <i>bool<i> true on success
    **/
    bool ReadIniFile(const std::string& sFilename);

    /** @brief Get the string value for the specified key in the specified section.
    *   @param sSection the section of the ini file the key live is
    *   @param sKey the key you want to retrieve the value of
    *   @param sDefault the default value of the key (this is returned if the key is not set)
    *   @return <i>string</i> the value of the key
    **/
    const std::string& GetIniString(const std::string& sSection, const std::string& sKey, const std::string& sDefault) const;
    /** @brief Get the ini value for the specified key in the specified section.
    *   @param sSection the section of the ini file the key live is
    *   @param sKey the key you want to retrieve the value of
    *   @param nDefault the default value of the key (this is returned if the key is not set)
    *   @return <i>int</i> the value of the key
    **/
	int GetIniInt(const std::string& sSection, const std::string& sKey, int nDefault) const;

	/** @brief Get the double value for the specified key in the specified section.
    *   @param sSection the section of the ini file the key live is
    *   @param sKey the key you want to retrieve the value of
    *   @param dDefault the default value of the key (this is returned if the key is not set)
    *   @return <i>double</i> the value of the key
    **/
	double GetIniDouble(const std::string& sSection, const std::string& sKey, double dDefault) const;

    /** @brief Writes the stored sections and keys into an ini file
    *   @param sFilename the path and filename of the ini file to write to
    *   @return <i>bool<i> true on success
    **/
    bool WriteIniFile(const std::string& sFilename);

    /** @brief  Creates or returns a section to store keys and values in
    *   @param sSectionName the name of the sectino
    *   @return <i>iniSection*</i> pointer to the created section.
    **/
	iniSection* CreateSection(const std::string& sSectionName);

	/** @brief Sets the value of the specified key. Creating both the key and section if needed
	*   @param sSectionName the name of the section
	*   @param sKey the key to set the value of
	*   @param sValue the value to set the key
	*   @return <i>void</i>
	**/
	void SetSectionValue(const std::string& sSectionName, const std::string& sKey, const std::string& sValue);

	/** @brief Sets the value of the specified key. Creating both the key and section if needed
	*   @param sSectionName the name of the section
	*   @param sKey the key to set the value of
	*   @param sValue the value to set the key
	*   @return <i>void</i>
	**/
	void SetSectionValue(const std::string& sSectionName, const std::string& sKey, int nValue);

	/** @brief Returns the number of entries in a given section
	*   @param sSectionName the name of the section
	*   @return <i>size_t</i> the number of entries
	**/
	size_t GetNumberOfSectionEntries(const std::string& sSectionName);


    /** @brief Gets an interator pointing to the first section of the ini file
    *   @return std::map<std::string,iniSection*>::iterator
    **/
    std::map<std::string,iniSection*>::iterator GetSectionBegin();

    /** @brief Gets an iterator pointing to the end of the sections in the ini file
    *   @return std::map<std::string,iniSection*>::iterator
    **/
	std::map<std::string,iniSection*>::iterator GetSectionEnd();

    /** @brief Get a pointer to an iniSection with name sSectionName
    *   @param sSectionName the section to find
    *   @return <i>iniSection*</i> pointer to the iniSection object that encapsulates that section
    **/
	iniSection* GetSection(const std::string& sSectionName);

	/** @brief Deletes a section if it exists
	*   @param sSectionName
	*   @return <i>bool</i> true if section is found and deleted
	**/
	bool DeleteSection(const std::string& sSectionName);
protected:
    std::map<std::string,iniSection*> m_mSections;
	std::ifstream m_if;
	std::ofstream m_of;


    void DeleteSections();
public:

};


