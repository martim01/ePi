#include <iostream>
#include <thread>
#include <chrono>
#include "json/json.h"


using namespace std;

int main(int argc, char* argv[])
{

    int i = 0;
    while(i < 200)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        Json::Value jsValue;
        jsValue["count"] = i;
        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        builder["indentation"] = "";

        std::unique_ptr<Json::StreamWriter> pWriter(builder.newStreamWriter());
        pWriter->write(jsValue, &std::cout);
        cout <<  std::endl;

        i++;

    }
    return 0;
}
