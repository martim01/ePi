#include "epiwriter.h"
#include <iostream>


epiWriter&  epiWriter::Get()
{
    static epiWriter epi;
    return epi;
}


void epiWriter::writeToStdOut(const Json::Value& jsValue)
{
    m_pWriter->write(jsValue, &std::cout);
    std::cout <<  std::endl;
}

epiWriter::epiWriter()
{
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "";
    m_pWriter = std::unique_ptr<Json::StreamWriter>(builder.newStreamWriter());
}


void epiWriter::writeToSStream(const Json::Value& jsValue, std::stringstream& ss)
{
    m_pWriter->write(jsValue, &ss);
}
