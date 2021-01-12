#include <iostream>
#include <iomanip>
#include <cstring>
#include "NetworkClient.h"

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#elif defined(__APPLE__)
#include <stdio.h>
//#include <CoreFoundation/CoreFoundation.h>
//#include <IOKit/IOKitLib.h>
//#include <IOKit/network/IOEthernetInterface.h>
//#include <IOKit/network/IONetworkInterface.h>
//#include <IOKit/network/IOEthernetController.h>
#elif defined(LINUX) || defined(linux) || defined(__unix__)
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

NetworkClient::NetworkClient(const std::string& host, const int port) {
    this->status = NetworkStatus::Ok;
    try {
        std::cout << "connect to " << host << ":" << port << std::endl;
        this->address = asio::ip::address::from_string(host);
        this->port = port;
        this->socket = new asio::ip::tcp::socket(this->socketContext);
        const asio::ip::tcp::endpoint endpoint(this->address, this->port);
        this->socket->connect(endpoint);
    }
    catch (asio::system_error& e) {
        std::cout << "Network error: " << asio::error_code(e.code()).message() << std::endl;
        this->status = NetworkStatus::Error;
    }
}

int NetworkClient::changeEndianBytes(int data) {
    auto bytes = static_cast<char*>(malloc(sizeof(int)));
    if (bytes == nullptr) {
        exit(-1);
    }
    std::memcpy(bytes, &data, sizeof(int));
    char tmp = 0;
    for (unsigned int i = 0; i < (sizeof(int) >> 1); i++) {
        tmp = bytes[i];
        bytes[i] = bytes[sizeof(int) - i - 1];
        bytes[sizeof(int) - i - 1] = tmp;
    }
    std::memcpy(&data, bytes, sizeof(int));
    std::free(bytes);
    return data;
}

NetworkStatus NetworkClient::getStatus() const {
    return this->status;
}

NetworkStatus NetworkClient::transData(const void* transData, const unsigned int length) {
    if (length <= 0 || transData == nullptr) {
        this->status = NetworkStatus::EmptyData;
        return NetworkStatus::EmptyData;
    }
    asio::error_code error;
    auto sendLength = write(*this->socket, 
                                  asio::buffer(transData, length), error);
    if (error.value() == 0 && sendLength == length) {
        this->status = NetworkStatus::Ok;
        return NetworkStatus::Ok;
    } else {
        this->status = NetworkStatus::Error;
        return NetworkStatus::Error;
    }
}

NetworkStatus NetworkClient::receiveData(void* receiveBuffer, const unsigned int length) {
    if (length <= 0 || receiveBuffer == nullptr) {
        this->status = NetworkStatus::EmptyData;
        return NetworkStatus::EmptyData;
    }
    asio::error_code error;
    const auto getLength = read(*this->socket, 
                                      asio::buffer(receiveBuffer, length), error);
    if (error.value() == 0 && getLength == length) {
        this->status = NetworkStatus::Ok;
        return NetworkStatus::Ok;
    } else {
        this->status = NetworkStatus::Error;
        return NetworkStatus::Error;
    }
}

NetworkClient::~NetworkClient() {
    try {
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
        Sleep(500);
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
        usleep(500000);
#endif
        //this->socket->shutdown(asio::ip::tcp::socket::shutdown_both);
        this->socket->close();
    } catch (asio::system_error& e) {
        std::cout << "Close network error: " << asio::error_code(e.code()).message() << std::endl;
    }
    delete(this->socket);
}

// get mac source code:
// https://wxwidgets.info/cross-platform-way-of-obtaining-mac-address-of-your-machine/ 
int MACAddressUtility::GetMACAddress(std::string& macString) {
    unsigned char result[7] = { 0 };
    int returnVal = -1;
    // Call appropriate function for each platform
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
    returnVal = GetMACAddressMSW(result);
#elif defined(__APPLE__)
    returnVal = GetMACAddressMAC(result);
#elif defined(LINUX) || defined(linux)
    returnVal = GetMACAddressLinux(result);
#endif
    // If platform is not supported then return error code
    std::stringstream ss;
    for (auto i : result)
        ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(i);
    macString.clear();
    macString = ss.str();
    return returnVal;
}

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
inline int MACAddressUtility::GetMACAddressMSW(unsigned char* result) {
    UUID uuid;
    if (UuidCreateSequential(&uuid) == RPC_S_UUID_NO_ADDRESS) return -1;
    memcpy(result, reinterpret_cast<char*>(uuid.Data4 + 2), 6);
    return 0;
}
#elif defined(__APPLE__)
//static kern_return_t FindEthernetInterfaces(io_iterator_t* matchingServices) {
//    kern_return_t		kernResult;
//    CFMutableDictionaryRef	matchingDict;
//    CFMutableDictionaryRef	propertyMatchDict;
//
//    matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);
//
//    if (NULL != matchingDict) {
//        propertyMatchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
//                                                      &kCFTypeDictionaryKeyCallBacks,
//                                                      &kCFTypeDictionaryValueCallBacks);
//
//        if (NULL != propertyMatchDict) {
//            CFDictionarySetValue(propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue);
//            CFDictionarySetValue(matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
//            CFRelease(propertyMatchDict);
//        }
//    }
//    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, matchingServices);
//    return kernResult;
//}
//
//static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8* MACAddress, UInt8 bufferSize) {
//    io_object_t		intfService;
//    io_object_t		controllerService;
//    kern_return_t	kernResult = KERN_FAILURE;
//
//    if (bufferSize < kIOEthernetAddressSize) {
//        return kernResult;
//    }
//
//    bzero(MACAddress, bufferSize);
//
//    while (intfService = IOIteratorNext(intfIterator)) {
//        CFTypeRef	MACAddressAsCFData;
//
//        // IONetworkControllers can't be found directly by the IOServiceGetMatchingServices call, 
//        // since they are hardware nubs and do not participate in driver matching. In other words,
//        // registerService() is never called on them. So we've found the IONetworkInterface and will 
//        // get its parent controller by asking for it specifically.
//
//        // IORegistryEntryGetParentEntry retains the returned object, so release it when we're done with it.
//        kernResult = IORegistryEntryGetParentEntry(intfService,
//                                                   kIOServicePlane,
//                                                   &controllerService);
//
//        if (KERN_SUCCESS != kernResult) {
//            printf("IORegistryEntryGetParentEntry returned 0x%08x\n", kernResult);
//        } else {
//            // Retrieve the MAC address property from the I/O Registry in the form of a CFData
//            MACAddressAsCFData = IORegistryEntryCreateCFProperty(controllerService,
//                                                                 CFSTR(kIOMACAddress),
//                                                                 kCFAllocatorDefault,
//                                                                 0);
//            if (MACAddressAsCFData) {
//                CFShow(MACAddressAsCFData); // for display purposes only; output goes to stderr
//
//                // Get the raw bytes of the MAC address from the CFData
//                CFDataGetBytes((CFDataRef)MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress);
//                CFRelease(MACAddressAsCFData);
//            }
//
//            // Done with the parent Ethernet controller object so we release it.
//            (void)IOObjectRelease(controllerService);
//        }
//
//        // Done with the Ethernet interface object so we release it.
//        (void)IOObjectRelease(intfService);
//    }
//
//    return kernResult;
//}

int MACAddressUtility::GetMACAddressMAC(unsigned char* result) {
    //io_iterator_t	intfIterator;
    //kern_return_t	kernResult = KERN_FAILURE;
    //do {
    //    kernResult = ::FindEthernetInterfaces(&intfIterator);
    //    if (KERN_SUCCESS != kernResult) break;
    //    kernResult = ::GetMACAddress(intfIterator, (UInt8*)result, 6);
    //} while (false);
    //(void)IOObjectRelease(intfIterator);
    //return kernResult;
    result[0] = 'O';
    result[1] = 'S';
    result[2] = 'X';
    return 0;
}

#elif defined(LINUX) || defined(linux) || defined(__unix__)
int MACAddressUtility::GetMACAddressLinux(unsigned char* result) {
    struct ifreq ifr;
    struct ifreq* IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i;
    int ok = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) {
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) {
            if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
                    ok = 1;
                    break;
                }
            }
        }
    }

    shutdown(s, SHUT_RDWR);
    if (ok) {
        bcopy(ifr.ifr_hwaddr.sa_data, result, 6);
    } else {
        return -1;
    }
    return 0;
}
#endif