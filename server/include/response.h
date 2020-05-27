#pragma once
#include "json/json.h"

struct response
{
    response(unsigned short nCode=200) : nHttpCode(nCode){}
    response(const response& aResponse) : nHttpCode(aResponse.nHttpCode), jsonData(aResponse.jsonData){};
    response& operator=(const response& aResponse)
    {
        if(this != &aResponse)
        {
            nHttpCode = aResponse.nHttpCode;
            jsonData = aResponse.jsonData;
        }
        return *this;

    }

    unsigned short nHttpCode;
    Json::Value jsonData;
};
