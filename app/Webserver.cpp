#include "Webserver.h"
#include "../common/Utility.h"

using namespace simpleBlockchain::common;
using namespace std;

namespace simpleBlockchain::app
{

void Webserver::start()
{

    _listener.support(methods::GET, [ = ](http_request request) // http://127.0.0.1:2826/blocks
    {
        std::vector<utility::string_t> path = uri::split_path(uri::decode(request.relative_uri().path()));
        if (path.size() != 1 || path[0] != "blocks")
        {
            Utility::send_response<string>(request, "unknown URL for GET request", 401);
        }
        _work_queue.push({request, RequestType::GET_BLOCKS, ""});
    });


    _listener.support(methods::POST, [ = ](http_request request) // http://127.0.0.1:2826/transaction
    {
        std::vector<utility::string_t> path = uri::split_path(uri::decode(request.relative_uri().path()));
        if (path.size() != 1 || path[0] != "transaction")
        {
            Utility::send_response<string>(request, "unknown URL for Post request", 401);
        }
        _work_queue.push({request, RequestType::POST_TRANSACTION, request.extract_string().get()});

    });

    _listener.open().wait();

    cout << "Webserver ready to accept connections" << endl;
}

}
