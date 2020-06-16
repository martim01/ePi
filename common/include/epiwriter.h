#pragma once
#include "json/json.h"
#include <memory>

class epiWriter
{
    public:
        static epiWriter&  Get();
        void writeToStdOut(const Json::Value& jsValue);
        void writeToSStream(const Json::Value& jsValue, std::stringstream& ss);

    private:
        epiWriter();
        std::unique_ptr<Json::StreamWriter> m_pWriter;
};
