#pragma once
#include <cpprest/http_client.h>
#include <string>
#include <type_traits>
#include <chrono>

using namespace web;
using namespace web::http;
using namespace std::chrono;
using std::string;

namespace simpleBlockchain::common
{

class Utility
{
public:
    template<typename MsgTypeT>
    static void send_response(const http_request& request, const MsgTypeT& message, const status_code code)
    {
        http_response response(code);
        response.set_body(message);
        request.reply(response);
    }
    static string read_file(const string& path);
    static string binToHex(const unsigned char* const bin_ptr, uint32_t size);
};

template<typename CounterVarT, typename MeasureT = std::ratio<1>>
class Timer
{
public:
    system_clock::time_point _timeCreated;
    CounterVarT& _counterVar;

    explicit Timer(CounterVarT& counterVar): _timeCreated(system_clock::now()), _counterVar(counterVar)
    {
    }

    ~Timer()
    {
        const duration<double, MeasureT> duration = system_clock::now() - _timeCreated;
        _counterVar = duration.count();
    }
};

using TimerSecs = Timer<uint64_t>;
using TimerMillis = Timer<uint64_t, std::milli>;
using TimerMicros = Timer<uint64_t, std::micro>;

}
