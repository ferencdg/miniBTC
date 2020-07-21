#pragma once
#include <cpprest/http_msg.h>
#include <string>

using namespace web;
using namespace web::http;
using std::string;

namespace simpleBlockchain::common
{

enum class RequestType {GET_BLOCKS, POST_TRANSACTION};

struct HttpRequestHolder
{
    http_request original_http_request;
    RequestType request_type;
    string payload;
};

}

