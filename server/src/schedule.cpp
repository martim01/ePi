#include "schedule.h"


Schedule::Schedule(const std::string& sUid, const Json::Value& jsData) : Resource("Schedule")
{
    m_json = jsData;
    m_json["uid"] = sUid;
}

Schedule::Schedule() : Resource("Schedule")
{

}

    protected:
        std::map<std::chrono::milliseconds, scheduleItem> m_mSchedule;
