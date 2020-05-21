#include "resource.h"
#include "utils.h"

Resource::Resource(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription) :
    m_sType(sType)
{
    m_json["label"] = sLabel;
    m_json["description"] = sDescription;
    m_json["uid"] = sUid;
    m_json["created"] = GetCurrentTimeAsString(false);
    m_json["type"] = sType;
}

Resource::Resource(const std::string& sType) :
    m_sType(sType)
{
}

Resource::Resource(const std::string& sType, const std::string& sUid, const Json::Value& jsData) : m_sType(sType),
m_json(jsData)
{
    m_json["uid"] = sUid;
    m_json["created"] = GetCurrentTimeAsString(false);
    m_json["type"] = sType;
}

Resource::Resource(const Json::Value& jsData) : m_json(jsData)
{

}

const Json::Value& Resource::GetJson() const
{
    return m_json;
}


const std::string& Resource::GetType() const
{
    return m_sType;
}

std::string Resource::GetUid() const
{
    return m_json["uid"].asString();
}

std::string Resource::GetLabel() const
{
    return m_json["label"].asString();
}


void Resource::UpdateJson(const Json::Value& jsData)
{
    m_json = jsData;
}
