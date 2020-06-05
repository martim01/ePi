#pragma once
#include "json/json.h"
class iniManager;


class Resources
{
    public:
        static Resources& Get();
        const Json::Value& GetJson() const { return m_jsData; }
        bool Load(const iniManager& iniConfig);
        bool IsValid() const { return m_bValid; }
    private:
        Resources();

        Json::Value m_jsData;
        bool m_bValid;
};
