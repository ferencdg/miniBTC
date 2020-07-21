#include "Utility.h"
#include <sstream>
#include <sodium.h>
#include <string>
#include <memory>
#include <cpprest/http_msg.h>

using namespace web;
using namespace web::http;
using std::string;
using std::unique_ptr;

namespace simpleBlockchain::common
{

string Utility::read_file(const string& path)
{
    std::ifstream stream(path);
    std::stringstream stringStream;
    stringStream << stream.rdbuf();
    return stringStream.str();
}

string Utility::binToHex(unsigned char const* const bin_ptr, uint32_t size)
{
    string tmpStr;
    tmpStr.resize(size * 2 + 1);
    // c++11 guarantees that the string stores it's characters in a contiguous memory area
    sodium_bin2hex(&tmpStr[0], size * 2 + 1, bin_ptr, size);
    return "0x" + tmpStr.substr(0, tmpStr.size() - 1);
}



}


