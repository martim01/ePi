#include "resources.h"
#include "log.h"
#include <fstream>
#include "inimanager.h"
#include "epiutils.h"

Resources& Resources::Get()
{
    static Resources res;
    return res;
}


Resources::Resources() : m_bValid(false)
{

}


bool Resources::Load(const iniManager& iniConfig)
{
    std::ifstream ifs;
    ifs.open(CreatePath(iniConfig.GetIniString("Paths", "Resources", "/var/ePi/resources"))+"resources.json", std::ifstream::in);
    if(!ifs.is_open())
    {
        pmlLog(pml::LOG_CRITICAL) << "Schedule\tCould not open resources file!" << std::endl;
		return false;
	}

	try
	{
        ifs >> m_jsData;

        if(m_jsData["schedules"].isArray() && m_jsData["files"].isArray() && m_jsData["playlists"].isArray())
        {
            m_bValid = true;
            return true;
        }
        return false;
    }
    catch(const Json::RuntimeError& e)
    {
        return false;
    }
    return false;
}
