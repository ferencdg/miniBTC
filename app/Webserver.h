#pragma once
#include <cpprest/http_listener.h>
#include <cpprest/http_client.h>
#include <string>
#include "../common/BlockingQueue.h"
#include "../common/HttpRequestHolder.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace simpleBlockchain::common;
using std::string;

namespace simpleBlockchain::app
{

class Webserver
{
public:
    Webserver(const string& http_server_url, BlockingQueue<HttpRequestHolder>& work_queue): _listener(http_server_url), _work_queue(work_queue) {}
    void start();

private:
    http_listener _listener;
    BlockingQueue<HttpRequestHolder>& _work_queue;
    void send_response(const http_request& request, const string& message, const status_code code);
};

}
