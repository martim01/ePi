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

//#include <wx\dlimpexp.h>



/**
*	@brief class that contains data loaded from a section of an ini file
*	@author Matthew Martin
*/

typedef std::map<std::string,std::string> mapIniData;
typedef std::map<std::string,std::string>::iterator itIniData;

class iniSection{
    friend class iniManager;

public:

	/** @brief Get and iterator pointing to the beginning of the data
	*	@return <i>itIniData</i> the iterator
	**/
    std::map<std::string,std::string>::const_iterator GetDataBegin() const;

	/** @brief Get and iterator pointing to the end of the data
	*	@return <i>itIniData</i> the iterator
	**/
	std::map<std::string,std::string>::const_iterator GetDataEnd() const;



	/** @brief Get the name of the section
	*	@return <i>std::string</i> the section name
	**/
	const std::string& GetSectionName()
	{	return m_sSectionName;	}

	/** @brief Get the string value of data with key sKey
	*	@param sKey the key name of the data
	*	@param sDefault the value to return of the data does not exist
	*	@return <i>std::string</i> the value of the data
	**/
    const std::string& GetString(const std::string& sKey, const std::string& sDefault);

	/** @brief Get the int value of data with key sKey
	*	@param sKey the key name of the data
	*	@param nDefault the value to return of the data does not exist
	*	@return <i>int</i> the value of the data
	**/
	int GetInt(const std::string& sKey, int nDefault);

	/** @brief Get the double value of data with key sKey
	*	@param sKey the key name of the data
	*	@param dDefault the value to return of the data does not exist
	*	@return <i>double</i> the value of the data
	**/
	double GetDouble(const std::string& sKey, double dDefault);

	/** @brief Get a count of the number of bits of data
	*	@return <i>size_t</> thee count
	**/
	size_t GetNumberOfEntries();

	/** @brief Sets the key,value pair in m_mData
	*	@param sKey the key
	*	@param sValue the value
	**/
	void SetValue(const std::string& sKey, const std::string& sValue);

    /** @brief Returns a const_iterator pointing to the key,data pair of the given key
    *   @param sKey the key to search fro
    *   @return <i>map<std::string,std::string>::const_iterator
    **/
	std::map<std::string,std::string>::const_iterator FindData(const std::string& sKey) const;

	protected:
	std::map<std::string,std::string> m_mSectionData;	///< map containing key, value pairs */
	std::string m_sSectionName;					///< the section name */

	/** Constructor
	*	@param sSection the section name
	**/
    explicit iniSection(const std::string& sSection);

	/** Destructor
	**/
	~iniSection();

	/** Reads in the section from the supplied ifstream
	*	@param pif the opened ifstream
	*	@return <i>std::string</i> the name of the section??
	**/
    std::string ReadSection(std::ifstream* pif);

	/** Writes the data to the supplied ofstream
	*	@param pof the opened ofstream
	**/
	void WriteSection(std::ofstream* pof);


};

