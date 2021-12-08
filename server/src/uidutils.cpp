#include "uidutils.h"
#include "guid.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "epiutils.h"

static uuid_t NameSpace_OID = { /* 6ba7b812-9dad-11d1-80b4-00c04fd430c8 */
       0x6ba7b812,
       0x9dad,
       0x11d1,
       0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
   };

std::string CreateGuid(const std::string& sName)
{
    uuid_t guid;
    uuid_create_md5_from_name(&guid, NameSpace_OID, sName.c_str(), sName.length());

    std::stringstream ss;
    ss << std::hex
       << std::setw(8) << std::setfill('0') << guid.time_low << "-"
       << std::setw(4) << std::setfill('0') << guid.time_mid << "-"
       << std::setw(4) << std::setfill('0') << guid.time_hi_and_version << "-"
       << std::setw(2) << std::setfill('0') << (int)guid.clock_seq_hi_and_reserved
       << std::setw(2) << std::setfill('0') << (int)guid.clock_seq_low << "-"
       << std::setw(2) << std::setfill('0') << (int)guid.node[0]
       << std::setw(2) << std::setfill('0') << (int)guid.node[1]
       << std::setw(2) << std::setfill('0') << (int)guid.node[2]
       << std::setw(2) << std::setfill('0') << (int)guid.node[3]
       << std::setw(2) << std::setfill('0') << (int)guid.node[4]
       << std::setw(2) << std::setfill('0') << (int)guid.node[5];

    return ss.str();
    /*std::array<char,40> output;
    snprintf(output.data(), output.size(), "%08x-%04hx-%04hx-%02x%02x-%02x%02x%02x%02x%02x%02x",
             guid.time_low, guid.time_mid, guid.time_hi_and_version, guid.clock_seq_hi_and_reserved, guid.clock_seq_low,
             guid.node[0], guid.node[1], guid.node[2], guid.node[3], guid.node[4], guid.node[5]);
    return std::string(output.data());*/
}


std::string CreateGuid()
{
    return CreateGuid(GetCurrentTimeAsString(true));
}

