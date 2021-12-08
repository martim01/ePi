#include "json/json.h"
#include <string>
#include <initializer_list>


extern bool CheckJson(const Json::Value& jsObject, std::initializer_list<std::string> lstAllowed);
extern bool JsonMemberExistsAndIsNull(const Json::Value& jsObject, const std::string& sMember);
extern bool JsonMemberExistsAndIsNotNull(const Json::Value& jsObject, const std::string& sMember);
extern Json::Value ConvertToJson(const std::string& str);
extern void UpdateJsonObject(Json::Value& dest, const Json::Value& source);

