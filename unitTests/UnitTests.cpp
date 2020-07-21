#include "gtest/gtest.h"
#include "../thirdParty/nlohmann/json.hpp"
#include "../common/BlockchainDataStructs.h"
#include "../common/Utility.h"
#include "../common/Configuration.h"
#include <iostream>

using namespace simpleBlockchain::common;
using njson = nlohmann::json;
using std::cout;
using std::endl;

namespace simpleBlockchain::unitTests
{

class BlockchainFixture: public testing::Test
{
public:
    BlockchainFixture(): _configuration(Configuration::getConfiguration())
    {
        // creating the genesis block with the corresponding transaction
        Block block{};
        block.transaction.fromJson(njson::parse(Utility::read_file("../../unitTests/testFiles/transaction.txt")));
        _blockchain.blocks.push_back(block);
        _blockchain.transactions_map[block.transaction.hash] = 0;

        // creating the next transaction
        _transactionToCheck.fromJson(njson::parse(Utility::read_file("../../unitTests/testFiles/transaction2.txt")));

        // switching off signature check by default
        _configuration.skipSignatureCheck = true;
    }

    Configuration& _configuration;
    Blockchain _blockchain;
    Transaction _transactionToCheck;
};

TEST_F(BlockchainFixture, TransactionValidationCorrectTransaction)
{
    _configuration.skipSignatureCheck = false;
    TransactionValidationResult transactionValidationResult = _transactionToCheck.validate(_blockchain);
    ASSERT_TRUE(transactionValidationResult.isSuccess) << transactionValidationResult.errorMsg;
}


TEST_F(BlockchainFixture, InputTransactionNotOnBlockchain)
{
    _transactionToCheck.inputs[0].hash[0] = 42;
    TransactionValidationResult transactionValidationResult = _transactionToCheck.validate(_blockchain);
    ASSERT_FALSE(transactionValidationResult.isSuccess) << transactionValidationResult.errorMsg;
}

TEST_F(BlockchainFixture, OutputTransactionIndexTooLarge)
{
    _transactionToCheck.inputs[0].index = 2;
    TransactionValidationResult transactionValidationResult = _transactionToCheck.validate(_blockchain);
    ASSERT_FALSE(transactionValidationResult.isSuccess) << transactionValidationResult.errorMsg;
}


TEST_F(BlockchainFixture, OutputSpent)
{
    _blockchain.blocks[0].transaction.outputs[0].spent = true;
    TransactionValidationResult transactionValidationResult = _transactionToCheck.validate(_blockchain);
    ASSERT_FALSE(transactionValidationResult.isSuccess) << transactionValidationResult.errorMsg;
}

TEST_F(BlockchainFixture, OutputValueLargerThanInputValue)
{
    _transactionToCheck.outputs[0].amount += 1;
    TransactionValidationResult transactionValidationResult = _transactionToCheck.validate(_blockchain);
    ASSERT_FALSE(transactionValidationResult.isSuccess) << transactionValidationResult.errorMsg;
}

}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
