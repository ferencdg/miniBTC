#pragma once
#include <string>
#include "../common/BlockingQueue.h"
#include "../common/HttpRequestHolder.h"
#include "../common/BlockchainDataStructs.h"

using namespace simpleBlockchain::common;

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
    Block mineBlock(const Transaction& transaction);
};

}
