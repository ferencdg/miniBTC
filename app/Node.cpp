#include "Node.h"
#include "Webserver.h"
#include "../common/Utility.h"
#include "../thirdParty/nlohmann/json.hpp"
#include "../common/Configuration.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;
using namespace simpleBlockchain::app;
using namespace simpleBlockchain::common;
namespace po = boost::program_options;
using njson = nlohmann::json;

namespace simpleBlockchain::app
{

void Node::start()
{
    while (true)
    {
        HttpRequestHolder request_holder;
        _work_queue.waitAndPop(request_holder);
        if (request_holder.request_type == RequestType::GET_BLOCKS) // processing a 'blocks' request
        {
            Utility::send_response<string>(request_holder.original_http_request, _blockchain.toJson().dump(4), 200);
            continue;
        }
        else // processing a 'transaction' request
        {
            cout << "new transaction received, please wait while verifying..." << endl;
            // building up the Transaction object from the json provided in the request and also
            // doing basic validation to make sure the message format is correct
            Transaction transaction;
            try
            {
                JsonParseResult parse_result = transaction.fromJson(njson::parse(request_holder.payload));
                if (!parse_result.isSuccess)
                {
                    Utility::send_response<string>(request_holder.original_http_request, "\n\n" + parse_result.errorMsg + "\n\n", 501);
                    continue;
                }
            }
            catch (...) // 3rd party library njson::parse can throw arbitrary exception
            {
                Utility::send_response<string>(request_holder.original_http_request, "\n\n invalid json format\n\n", 501);
                continue;
            }

            // making sure the transaction can be safely included into the blockchain
            TransactionValidationResult validationResult = transaction.validate(_blockchain);
            if (!validationResult.isSuccess)
            {
                Utility::send_response<string>(request_holder.original_http_request, "\n\n" + validationResult.errorMsg + "\n\n", 501);
                continue;
            }

            // mine the block
            optional<Block> minedBlockOptional = mineBlock(transaction);

            // nonce turned around and we still haven't found the right hash
            if (!minedBlockOptional)
            {
                Utility::send_response<string>(request_holder.original_http_request, "\n\nunable to include this transaction into the blockchain at the current difficulty level\n\n", 501);
                continue;
            }

            Block& minedBlock = minedBlockOptional.get();
            vector<Block>& blocks = _blockchain.blocks;

            // now we can mark the refered transaction output's as spent
            for (const Input& input : transaction.inputs)
                blocks[_blockchain.transactions_map[input.hash]].transaction.outputs[input.index].spent = true;

            // adding the new block to the blockchain
            blocks.push_back(minedBlock);

            // adding the transaction to the transaction map
            _blockchain.transactions_map[transaction.hash] = blocks.size() - 1;

            // send resonse back to the client
            Utility::send_response<string>(request_holder.original_http_request, "\n\nblock mined\n\n", 200);

            cout << "new block added to the blockchain:\n\n" << minedBlock.toJson().dump(4) << endl << endl;
        }
    };
}

// this method could be put into its own interface called MinerInterface
// we could have multiple miner implementations like SingleThreadedMiner or MultiThreadedMiner
optional<Block> Node::mineBlock(const Transaction& transaction)
{
    Block blockToMine{}; // initializes everything to 0, including the nonce
    vector<Block>& blocks = _blockchain.blocks;
    blockToMine.height = blocks.size();
    blockToMine.transaction = transaction;
    bool isCurrentBlockGenesis = blocks.empty();
    if (!isCurrentBlockGenesis)
    {
        blockToMine.previousHash = blocks[blocks.size() - 1].hash;
        blockToMine.previousHashHex = blocks[blocks.size() - 1].hashHex;
    }
    uint8_t difficulty = Configuration::getConfiguration().baseDifficulty + (blockToMine.height % 3);

    // change the nonce until we find the right hash
    uint64_t millisecsElapsed;
    // this part of the hash will never change, so we will reuse it
    crypto_hash_sha256_state state = blockToMine.getInitHash();
    {
        TimerMillis timer(millisecsElapsed);
        do
        {
            ++blockToMine.nonce;
            // the nonce can turn around and become zero again in case the difficulty is high enough
            if (!blockToMine.nonce)
                return boost::none;

            blockToMine.recalculateHash(state);
        }
        while (!blockToMine.passDifficultyTest(difficulty));
    }
    cout << "blocked mined in " << millisecsElapsed / 1000 << " seconds " << millisecsElapsed % 1000 << " milliseconds with nonce(" << blockToMine.nonce << ") with overall hash rate of " << (blockToMine.nonce / 1000) / (millisecsElapsed / 1000) << "Hz/s" << endl;

    return optional<Block>(blockToMine);
}

}

int main(int argc, char* argv[])
{
    Configuration& configuration = Configuration::getConfiguration();

    // specifying command line options
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("web-server-ip,i", po::value<string>(&configuration.webServerHttpAddress)->default_value("http://127.0.0.1:2826")->required(), "the web server address when the clients can send http requests")
    ("base-difficulty,d", po::value<uint16_t>(&configuration.baseDifficulty)->default_value(5)->required(), "mining base difficulty");

    // processing command line options
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    if (vm.count("help"))
    {
        cout << "Usage: options_description [options]\n";
        cout << desc;
        return 0;
    }
    po::notify(vm);
    cout << "Node starting with base difficult of:" << to_string(configuration.baseDifficulty) << endl;

    // the web server and the node will communicate through this unbounded thread-safe blocking queue
    BlockingQueue<HttpRequestHolder> work_queue;

    // starting up the web server that will spawn multiple threads each able to receive http requests
    Webserver webserver(configuration.webServerHttpAddress, work_queue);
    webserver.start();

    // starting up the node instance
    // the  node is responsible for both validating the transactions and also mining them
    Node node(work_queue);
    node.start();
}
