#include "Configuration.h"

namespace simpleBlockchain::common
{

Configuration::Configuration()
{

}

Configuration& Configuration::getConfiguration()
{
    static Configuration configuration;
    return configuration;
}

}
