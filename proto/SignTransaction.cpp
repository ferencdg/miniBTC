#include <iostream>
#include <sodium.h>
#include "../common/BlockchainDataStructs.h"
#include "../common/Utility.h"

using namespace std;
using namespace simpleBlockchain::common;
using json = nlohmann::json;

Transaction parseTransaction(const string& filePath)
{
    string fileContent = Utility::read_file(filePath);
    Transaction transaction;
    nlohmann::json j = nlohmann::json::parse(fileContent);
    transaction.fromJson(j);
    nlohmann::json j2 = transaction.toJson();

    return transaction;
}

void generateKey()
{
    unsigned char pk[crypto_sign_ed25519_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_ed25519_SECRETKEYBYTES];
    crypto_sign_ed25519_keypair(pk, sk);

    cout << "public key: " << Utility::binToHex(pk, crypto_sign_ed25519_PUBLICKEYBYTES)  << endl;
    cout << "private key: " << Utility::binToHex(sk, crypto_sign_ed25519_SECRETKEYBYTES)  << endl;
    cout << endl;
}

void signMessage()
{
    //string skString = "8292e116b8609a7ccc3599363bc60f5f9e3646c2fce1e062490875dbae1b624eb8e4faf9bbf2e536f2b34e83577991df54a2234764a02434c9d03bf65c212666"; // generated by me
    //string skString = "32605c6eaaf3a62cbc72a9cb09be8e86aa859fd46f3924a0f97002fbe26e3e3093463e3fee4c0ad27a672c34ecd015e9eb09ae61dbe075ba79ed2425ef029cc7"; // generated by me
    string skString = "ae198c58212aca78fc0e8fb749241dbef5b399b3fc7a5c0d7232112244a1012b327f025d015ccba17d4a8b3641d7dc06e3ebe74b46aa61238ee2a3823666242c";
    //string skString = "77967261a7f23fdd2d4e9abf68d7d98312f83f7638f506b0039e332b15a62a2aee0edfbe581fa81481e858f75fda3414cd3a655ff17ec987af775e666412bf1f"; // came with the excercise
    array<unsigned char, crypto_sign_ed25519_SECRETKEYBYTES> sk;
    sodium_hex2bin(&sk[0], crypto_sign_ed25519_SECRETKEYBYTES, &skString[0], skString.size(), nullptr, nullptr, nullptr);

    //string pkString = "b8e4faf9bbf2e536f2b34e83577991df54a2234764a02434c9d03bf65c212666"; // generated by me
    //string pkString = "93463e3fee4c0ad27a672c34ecd015e9eb09ae61dbe075ba79ed2425ef029cc7"; // generated by me
    string pkString = "327f025d015ccba17d4a8b3641d7dc06e3ebe74b46aa61238ee2a3823666242c";
    //string pkString = "ee0edfbe581fa81481e858f75fda3414cd3a655ff17ec987af775e666412bf1f"; // came with the excercise
    array<unsigned char, crypto_sign_ed25519_PUBLICKEYBYTES> pk;
    sodium_hex2bin(&pk[0], crypto_sign_ed25519_PUBLICKEYBYTES, &pkString[0], pkString.size(), nullptr, nullptr, nullptr);

    Transaction transaction = parseTransaction("../../proto/transaction2.txt");

    unsigned char signed_message[crypto_sign_ed25519_BYTES];
    unsigned long long signed_message_len;
    crypto_sign_ed25519_detached(signed_message, &signed_message_len, &transaction.hash[0], transaction.hash.size(), &sk[0]);

    if (crypto_sign_ed25519_verify_detached(&signed_message[0], &transaction.hash[0], transaction.hash.size(), &pk[0]) != 0)
    {
        cout << "incorrect signature";
        exit(1);
    }

    cout << "signature: " << endl << Utility::binToHex(&signed_message[0], crypto_sign_ed25519_BYTES) << endl;
    cout << endl;
}

void hashComparison()
{

    unsigned char hash1[crypto_generichash_BYTES];
    crypto_generichash(hash1, sizeof hash1, (const unsigned char*)"abcd", 22, nullptr, 0);

    unsigned char hash2[crypto_generichash_BYTES];
    crypto_generichash_state state;
    crypto_generichash_init(&state, nullptr, 0, sizeof(hash2));
    crypto_generichash_update(&state, (const unsigned char*)"ab", 2);
    crypto_generichash_update(&state, (const unsigned char*)"cd", 2);
    crypto_generichash_final(&state, hash2, sizeof(hash2));
    if (hash1 != hash2)
        cout << "hash is not equal" << endl;
}


int main()
{
    signMessage();
    // generateKey();
    // parseTransaction("../../proto/transaction.txt");
}
