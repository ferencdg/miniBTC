#pragma once

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <sodium.h>
#include "../thirdParty/nlohmann/json.hpp"
#include <string.h>

using njson = nlohmann::json;
using std::string;
using std::array;
using std::vector;
using std::unordered_map;
using std::unordered_set;

namespace simpleBlockchain::common
{

typedef std::array<uint8_t, 32> Hash;
typedef std::array<uint8_t, 64> Signature;
typedef std::array<uint8_t, 32> PubKey;

struct OperationResult
{
    bool isSuccess;
    string errorMsg;
};

using JsonParseResult = OperationResult;
using TransactionValidationResult = OperationResult;

struct Input
{
    Hash hash;
    // for convenience we store the hex representation of the hashes and signatures too
    // in production we might want to calculate them on-the-fly instead in order to save memory
    string hashHex;
    uint32_t index;
    Signature sig;
    string sigHex;
};

struct Output
{
    uint32_t amount;
    PubKey   key;
    string keyHex;
    bool spent;
};

struct Blockchain;
struct Transaction
{
    Hash hash;
    string hashHex;
    vector<Input> inputs;
    vector<Output> outputs;
    JsonParseResult fromJson(njson j);
    njson toJson() const;
    TransactionValidationResult validate(const Blockchain& blockchain) const;
    inline void recalculateHash();
};

struct Block
{
    Hash hash;
    string hashHex;
    Hash previousHash;
    string previousHashHex;
    uint64_t height;
    uint64_t nonce;
    Transaction transaction;
    njson toJson() const;
    crypto_hash_sha256_state getInitHash() const;
    void recalculateHash(crypto_hash_sha256_state state);
    bool passDifficultyTest(uint8_t difficulty) const;
};

// hash function for std::array
template<int arrSize>
struct ArrayKeyHasher
{
    std::size_t operator()(const std::array<uint8_t, arrSize>& arr) const
    {
        return boost::hash_range(arr.begin(), arr.end());;
    }
};

struct Blockchain
{
    // key: transaction hash
    // value: block's id/height where the transaction was mined
    unordered_map<std::array<uint8_t, 32>, uint64_t, ArrayKeyHasher<32>> transactions_map;
    vector<Block> blocks;
    njson toJson() const;
};

}
