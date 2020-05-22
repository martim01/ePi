#pragma once
#include <string>
#include "json/json.h"


class Resource
{
    public:
        Resource(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription);
        Resource(const std::string& sType);
        Resource(const std::string& sType, const std::string& sUid, const Json::Value& jsData);
        Resource(const Json::Value& jsData);

        virtual void UpdateJson(const Json::Value& jsData);
        virtual ~Resource(){}

        virtual  void InitJson(){};

        std::string GetUid() const;
        std::string GetLabel() const;

        const Json::Value& GetJson() const;

        const std::string& GetType() const;

        void Lock(bool bLock);
        bool IsLocked() const;

    protected:
        std::string m_sType;
        Json::Value m_json;



};

