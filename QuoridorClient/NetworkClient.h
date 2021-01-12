#pragma once
#include <string>
#include <asio.hpp>

enum class NetworkStatus {
    Ok,
    EmptyData,
    Error,
};

class NetworkClient {
private:
    asio::ip::address address;
    int port = 5620;
    NetworkStatus status;
    asio::io_context socketContext;
    asio::ip::tcp::socket* socket = nullptr;

public:
    NetworkClient(const std::string& host = "127.0.0.1", const int port = 5620);
    ~NetworkClient();
    NetworkStatus getStatus() const;
    NetworkStatus transData(const void* transData, const unsigned int length = 0);
    NetworkStatus receiveData(void* receiveBuffer, const unsigned int length = 0);
    static int changeEndianBytes(int data);
};

// get mac source code:
// https://wxwidgets.info/cross-platform-way-of-obtaining-mac-address-of-your-machine/ 
class MACAddressUtility {
public:
    static int GetMACAddress(std::string& macString);
private:
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
    static int GetMACAddressMSW(unsigned char* result);
#elif defined(__APPLE__)
    static int GetMACAddressMAC(unsigned char* result);
#elif defined(LINUX) || defined(linux) || defined(__unix__)
    static int GetMACAddressLinux(unsigned char* result);
#endif
};