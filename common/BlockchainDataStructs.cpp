#include "BlockchainDataStructs.h"
#include <algorithm>
#include "Configuration.h"
#include "Utility.h"
#include <string>

using std::to_string;

namespace simpleBlockchain::common
{

TransactionValidationResult Transaction::validate(const Blockchain& blockchain) const
{
    int valueOfAllInputs = 0;
    int valueOfAllOutputs = 0;
    bool isCurrentBlockGenesis = blockchain.blocks.empty();
    // validating inputs
    // input can be anything for the Genesis block, so we will not perform any input validation on the Genesis block
    if (!isCurrentBlockGenesis)
        for (const Input& input : inputs)
        {
            // check if the transaction refered by the input is on the blockchain
            if (blockchain.transactions_map.find(input.hash) == blockchain.transactions_map.end())
                return {false, string("transaction with hash: ") + input.hashHex + " cannot be found on the blockchain"};

            // retrieve the refered transaction
            uint64_t blockId = blockchain.transactions_map.at(input.hash);
            const Transaction& transaction = blockchain.blocks[blockId].transaction;

            // check if the refered transaction has the output index
            if (input.index >= transaction.outputs.size())
                return {false, string("transaction with hash: ") + input.hashHex + " doesn't have an output index " + to_string(input.index)};

            // check if the refered transaction output is spent
            const Output& output = transaction.outputs[input.index];
            if (output.spent)
                return {false, string("transaction with hash: ") + input.hashHex + " with output index " + to_string(input.index) + " is already spent"};

            // check if the signiture is correct
            bool skip = Configuration::getConfiguration().skipSignatureCheck;
            if (!Configuration::getConfiguration().skipSignatureCheck && crypto_sign_ed25519_verify_detached(&input.sig[0], &hash[0], hash.size(), &output.key[0]) != 0)
            {
                return {false, string("signiture is incorrect for transaction with hash: ") + input.hashHex + " with output index " + to_string(input.index)};
            }
            valueOfAllInputs += output.amount;
        }

    // validating outputs
    int outputId = 0;
    for (const Output& output : outputs)
    {
        if (output.amount <= 0)
            return {false, string("value is non-positive for transaction with hash: ") + hashHex + " with output index " + to_string(outputId++)};

        valueOfAllOutputs += output.amount;
    }
    if (!isCurrentBlockGenesis && valueOfAllOutputs > valueOfAllInputs)
        return {false, string("value of all outputs exceeds value of all inputs; value of all outputs: ") + to_string(valueOfAllOutputs) + "; value of all inputs: " + to_string(valueOfAllInputs)};

    return {true, ""};
}

void Transaction::recalculateHash()
{
    crypto_hash_sha256_state state;
    crypto_hash_sha256_init(&state);
    for (const Input input : inputs)
    {
        crypto_hash_sha256_update(&state, &input.hash[0], input.hash.size());
        // Below we will add an uint32_t to the hash, however some CPU use little-endian while others use big-endian representation, so
        // the hash might be different on different CPUs. In a real production application, we should conver integers to the same representation.
        crypto_hash_sha256_update(&state, (const unsigned char*)&input.index, sizeof(input.index));
    }
    for (const Output output : outputs)
    {
        crypto_hash_sha256_update(&state, (const unsigned char*)&output.amount, sizeof(output.amount));
        crypto_hash_sha256_update(&state, &output.key[0], output.key.size());
    }
    crypto_hash_sha256_final(&state, &hash[0]);
    hashHex = Utility::binToHex(&hash[0], hash.size());
}

njson Transaction::toJson() const
{
    njson j;
    for (const Input& input : inputs)
    {
        njson jInput;

        jInput["hash"] = input.hashHex;
        jInput["index"] = input.index;
        jInput["sig"] = input.sigHex;

        j["inputs"].push_back(jInput);
    }

    for (const Output& output : outputs)
    {
        njson jOutput;

        jOutput["amount"] = output.amount;
        jOutput["key"] = output.keyHex;

        j["outputs"].push_back(jOutput);
    }

    j["hash"] = hashHex;

    return j;
}

JsonParseResult Transaction::fromJson(njson j)
{
    // parsing inputs
    if (!j.contains("inputs") || !j["inputs"].is_array())
        return {false, "transaction doesn't contain inputs field, or it is not an array"};
    for (const njson& jInput : j["inputs"])
    {
        Input input;
        if (!jInput.contains("hash") || !jInput["hash"].is_string())
            return {false, "transaction input doesn't contain hash field, or it is not a string"};
        if (!jInput.contains("index") || !jInput["index"].is_number_integer())
            return {false, "transaction input doesn't contain index field, or it is not an integer"};
        if (!jInput.contains("sig") || !jInput["sig"].is_string())
            return {false, "transaction input doesn't contain sig field, or it is not a string"};

        // parsing hash
        string hashHex = jInput["hash"];
        input.hashHex = hashHex;
        if (hashHex.size() >= 2)
            hashHex = hashHex.substr(2);
        sodium_hex2bin(&input.hash[0], input.hash.size(), hashHex.c_str(), hashHex.size(), nullptr, nullptr, nullptr);

        // parsing index
        input.index = jInput["index"];

        // parsing sig
        string sigHex = jInput["sig"];
        input.sigHex = sigHex;
        if (sigHex.size() >= 2)
            sigHex = sigHex.substr(2);
        sodium_hex2bin(&input.sig[0], input.sig.size(), sigHex.c_str(), sigHex.size(), nullptr, nullptr, nullptr);

        inputs.push_back(input);
    }

    // parsing outputs
    if (!j.contains("outputs") || !j["outputs"].is_array())
        return {false, "transaction doesn't contain outputs field, or it is not an array"};
    for (const njson& jOutput : j["outputs"])
    {
        Output output;
        if (!jOutput.contains("amount") || !jOutput["amount"].is_number_integer())
            return {false, "transaction output doesn't contain amount field, or it is not an integer"};
        if (!jOutput.contains("key") || !jOutput["key"].is_string())
            return {false, "transaction output doesn't contain key field, or it is not a string"};

        // parsing amount
        output.amount = jOutput["amount"];

        // parsing key
        string keyHex = jOutput["key"];
        output.keyHex = keyHex;
        if (keyHex.size() >= 2)
            keyHex = keyHex.substr(2);
        sodium_hex2bin(&output.key[0], output.key.size(), keyHex.c_str(), keyHex.size(), nullptr, nullptr, nullptr);

        output.spent = false;
        outputs.push_back(output);
    }

    // calculating transaction hash
    recalculateHash();

    return{true, ""};
}

njson Block::toJson() const
{
    njson j;
    j["pervious"] = previousHashHex;
    j["hash"] = hashHex;
    j["nonce"] = nonce;
    j["height"] = height;
    j["transaction"] = transaction.toJson();
    return j;
}

crypto_hash_sha256_state Block::getInitHash() const
{
    crypto_hash_sha256_state state;
    crypto_hash_sha256_init(&state);

    crypto_hash_sha256_update(&state, &previousHash[0], previousHash.size());
    crypto_hash_sha256_update(&state, (const unsigned char*) &height, sizeof(height));
    crypto_hash_sha256_update(&state, &transaction.hash[0], transaction.hash.size());

    return state;
}

void Block::recalculateHash(crypto_hash_sha256_state state)
{
    // in order to speed up hashing, this function could run on multiple threads, each thread starting from a different nonce
    crypto_hash_sha256_update(&state, (const unsigned char*) &nonce, sizeof(nonce));

    crypto_hash_sha256_final(&state, &hash[0]);
    hashHex = Utility::binToHex(&hash[0], hash.size());
}

bool Block::passDifficultyTest(uint8_t difficulty) const
{
    for (int i = 2; i < std::min<size_t>(difficulty + 2, hashHex.size()); ++i) // hashHex starts with '0x'
        if (hashHex[i] != '0')
            return false;
    return true;
}

njson Blockchain::toJson() const
{
    njson j = {};
    for (const Block& block : blocks)
    {
        j.push_back(block.toJson());
    }
    return njson(j);
}

}

