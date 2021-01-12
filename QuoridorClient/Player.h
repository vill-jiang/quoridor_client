#pragma once
#include "../QuoridorUtils/QuoridorUtils.h"
#include <string>
#include <algorithm>
#include "NetworkClient.h"

namespace QuoridorUtils {
class Player {
private:
    std::string myKey;
public:
    Player(const std::string& key) {
        std::string macStr;
        MACAddressUtility::GetMACAddress(macStr);
        this->myKey = key + '_' + macStr;
        std::transform(this->myKey.begin(), this->myKey.end(), this->myKey.begin(), 
                       [](unsigned char c) { return std::toupper(c); });
        if (this->myKey.length() == 64) {  // 保证长度为64字节 
            return;
        } else if (this->myKey.length() > 64) {
            this->myKey = this->myKey.substr(0, 64);
        } else {
            const size_t len = this->myKey.length();
            for (unsigned int i = 0; i < 64 - len; i++) {
                this->myKey.push_back('_');
            }
        }
    }

    Player(Player& oldObj) {
        this->myKey = oldObj.getKey();
    }

    std::string getKey() const {
        return this->myKey;
    }

    virtual ~Player() = default;
    virtual Step nextStep(const ChessboardChange& newChange) = 0;
    virtual void restart() = 0;
};
}
