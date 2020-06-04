#define CATCH_CONFIG_MAIN


#include <iostream>
#include "catch2/catch.hpp"
#include "epicron.h"
#include <chrono>


TEST_CASE("Correct Cron Passes", "[classic]")
{
    CronJob job;
    std::string str("12 20 12 * * * *");
    REQUIRE(job.SetString(str) == true);
}

TEST_CASE("Incorrect Cron Fails", "[classic]")
{
    CronJob job;
    std::string str("x 20 12 * * * *");
    REQUIRE(job.SetString(str) == false);
}

TEST_CASE("Cron tm triggers at correct time", "[classic]")
{
    CronJob job;
    std::string str("12 20 12 3 6 ? *");

    job.SetString(str);

    std::tm tm{0};
    tm.tm_sec = 11;
    tm.tm_min = 20;
    tm.tm_hour = 12;
    tm.tm_mon = 5;
    tm.tm_mday = 3;
    tm.tm_year = 120;

    REQUIRE(job.JobNow(tm) == false);

    tm.tm_sec =12;
    REQUIRE(job.JobNow(tm) == true);

    tm.tm_sec =13;
    REQUIRE(job.JobNow(tm) == false);
}


TEST_CASE("Cron tp triggers at correct time", "[classic]")
{
    CronJob job;
    std::string str("12 20 12 3 6 ? *");

    job.SetString(str);


    REQUIRE(job.JobNow(std::chrono::system_clock::from_time_t(1591183211)) == false);
    REQUIRE(job.JobNow(std::chrono::system_clock::from_time_t(1591183212)) == true);
    REQUIRE(job.JobNow(std::chrono::system_clock::from_time_t(1591183213)) == false);
}

