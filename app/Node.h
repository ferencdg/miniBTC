#pragma once
#include <string>
#include "../common/BlockingQueue.h"
#include "../common/HttpRequestHolder.h"
#include "../common/BlockchainDataStructs.h"
#include <boost/optional.hpp>

using namespace simpleBlockchain::common;
using boost::optional;

namespace simpleBlockchain::app
{

using std::string;

class Node
{
public:
    Node(BlockingQueue<HttpRequestHolder>& work_queue): _work_queue(work_queue) {}
    void start();
private:
    BlockingQueue<HttpRequestHolder>& _work_queue;
    Blockchain _blockchain;
    optional<Block> mineBlock(const Transaction& transaction);
};

}
