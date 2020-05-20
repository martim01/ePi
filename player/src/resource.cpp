#include "resource.h"
#include "utils.h"

Resource::Resource(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription) :
    m_sType(sType)
{
    m_json["label"] = sLabel;
    m_json["description"] = sDescription;
    m_json["uid"] = sUid;
    m_json["created"] = GetCurrentTimeAsString(false);
}

Resource::Resource(const std::string& sType) :
    m_sType(sType)
{
}



const Json::Value& Resource::GetJson()
{
    UpdateJson();
    return m_json;
}


const std::string& Resource::GetType() const
{
    return m_sType;
}

const std::string& Resource::GetUid() const
{
    return m_json["uid"].asString();
}

std::string Resource::GetJsonParseError()
{
    return m_ssJsonError.str();
}

