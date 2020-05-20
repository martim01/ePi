#pragma once
#include <string>
#include "json/json.h"
#include <list>
#include <map>
#include <chrono>
#include <mutex>
#include <sstream>

class Resource
{
    public:
        Resource(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription);
        Resource(const std::string& sType);

        virtual ~Resource(){}

        const std::string& GetUid() const;

        const Json::Value& GetJson();

        const std::string& GetType() const;

        std::string GetJsonParseError();

    protected:
        virtual void UpdateJson()=0;
        std::string m_sType;
        Json::Value m_json;
        bool m_bIsOk;

        std::stringstream m_ssJsonError;

};

