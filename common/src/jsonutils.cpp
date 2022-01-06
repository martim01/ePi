#include "jsonutils.h"
#include <iostream>
#include "log.h"

void UpdateJsonObject(Json::Value& dest, const Json::Value& source)
{
    if(!dest.isObject() || !source.isObject())
    {
        return;
    }
    for(const auto& key : source.getMemberNames())
    {
        if(dest[key].type() == Json::objectValue && source[key].type() == Json::objectValue)
        {
            UpdateJsonObject(dest[key], source[key]);
        }
        else
        {
            dest[key] = source[key];
        }
    }
}

bool CheckJson(const Json::Value& jsObject, std::initializer_list<std::string> lstAllowed)
{
    for(Json::Value::const_iterator itParam = jsObject.begin(); itParam != jsObject.end(); ++itParam)
    {
        std::initializer_list<std::string>::iterator itList = lstAllowed.begin();
        for(; itList != lstAllowed.end(); ++itList)
        {
            if((*itList) == itParam.key().asString())
                break;
        }
        if(itList == lstAllowed.end())   //found a non allowed thing
        {
            return false;
        }
    }
    return true;
}


bool JsonMemberExistsAndIsNull(const Json::Value& jsObject, const std::string& sMember)
{
    return (jsObject.isMember(sMember) && jsObject[sMember].isNull());
}

bool JsonMemberExistsAndIsNotNull(const Json::Value& jsObject, const std::string& sMember)
{
    return (jsObject.isMember(sMember) && (jsObject[sMember].isNull()==false));
}


Json::Value ConvertToJson(const std::string& str)
{
    Json::Value jsData;
    try
    {
        std::stringstream ss;
        ss.str(str);

        ss >> jsData;

    }
    catch(const Json::RuntimeError& e)
    {
        pmlLog(pml::LOG_ERROR) << "Could not convert '" << str << "' to JSON: " << e.what();
    }
    catch(const Json::LogicError& e)
    {
        pmlLog(pml::LOG_ERROR) << "Could not convert '" << str << "' to JSON: " << e.what();
    }

    return jsData;
}

