#include "resource.h"
#include "utils.h"

Resource::Resource(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription) :
    m_sType(sType)
{
    m_json["label"] = sLabel;
    m_json["description"] = sDescription;
    m_json["uid"] = sUid;
    m_json["created"] = GetCurrentTimeAsIsoString();
    m_json["type"] = sType;
    m_json["locked"] = false;
}

Resource::Resource(const std::string& sType) :
    m_sType(sType)
{
    m_json["label"] = "";
    m_json["description"] = "";
    m_json["uid"] = "";
    m_json["created"] = GetCurrentTimeAsIsoString();
    m_json["type"] = "";
    m_json["locked"] = false;
}

Resource::Resource(const std::string& sType, const std::string& sUid, const Json::Value& jsData) : m_sType(sType),
m_json(jsData)
{
    m_json["uid"] = sUid;
    m_json["created"] = GetCurrentTimeAsIsoString();
    m_json["type"] = sType;
    m_json["locked"] = false;
}

Resource::Resource(const Json::Value& jsData) : m_json(jsData)
{
    m_json["locked"] = false;
}

const Json::Value& Resource::GetJson() const
{
    return m_json;
}


std::string Resource::GetType() const
{
    return m_json["type"].asString();
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

void Resource::Lock(bool bLock)
{
    m_json["locked"] = bLock;
}

bool Resource::IsLocked() const
{
    return m_json["locked"].asBool();
}
