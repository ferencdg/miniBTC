#pragma once

#include <string>

using std::string;

namespace simpleBlockchain::common
{

class Configuration
{
public:
    string webServerHttpAddress;
    string loggingConfigFilePath;
    uint16_t baseDifficulty;
    bool skipSignatureCheck = false;

    static Configuration& getConfiguration();

    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

private:
    Configuration();
};

}
