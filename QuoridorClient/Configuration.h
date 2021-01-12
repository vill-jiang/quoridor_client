#pragma once
#include <list>
#include <tuple>
#include "NetworkClient.h"
#include "../QuoridorUtils/QuoridorUtils.h"
#include "Player.h"

namespace QuoridorUtils {
enum class MessageType;
enum class UserStatus;

class Configuration {
private:
    NetworkClient* networkClient = nullptr;
    std::string playerKey;
    std::string playerName;
    int victoryRoundCount = 0;
    int defeatedRoundCount = 0;
    //std::list<std::pair<std::string, QuoridorUtils::Step>> histories;
    std::list<std::tuple<std::string, QuoridorUtils::Step, QuoridorUtils::GameStatus>> histories;
    int messageLength(const MessageType type) const;
    UserStatus getChange(ChessboardChange& newChange) const;
    UserStatus transDataAndGetChange(ChessboardChange& newChange, Step* step = nullptr) const;
    void record(const ChessboardChange& lastChange, const Step& myStep = Step());

public:
    Configuration(NetworkClient* net);  // 注意,由于socket对象不能深拷贝,故传入的指针必须在游戏结束后释放
    Configuration(const std::string& host = "127.0.0.1", const int port = 5620);
    void startGame(Player& player, std::vector<std::string>& logFiles, const bool logSave = true);
    std::string saveGameHistory() const;
    ~Configuration();
};
}



/*
 *
 * 棋子坐标如下:                 挡板坐标结构如下：
 * ┌-┬-┬-┬-┬-┬-┬-┬-┬-┐         ┌-┬-┬-┬-┬-┬-┬-┬-┬-┐9
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖9       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤8
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖8       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤7
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖7       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤6
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖6       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤5
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖5       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤4
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖4       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤3
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖3       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤2
 * ├-┼-┼-┼-┼-┼-┼-┼-┼-┤↖2       ├-┼-┼-┼-┼-┼-┼-┼-┼-┤1
 * └-┴-┴-┴-┴-┴-┴-┴-┴-┘↖1 y     └-┴-┴-┴-┴-┴-┴-┴-┴-┘0 y
 *x 1 2 3 4 5 6 7 8 9         x0 1 2 3 4 5 6 7 8 9
 * 
 *
 *
 * 
 *
 * 通信协议:
 * 客户端主动，服务器从动。服务器回复状态和接下来的字节数再发送数据。
 * 1. 验证开始
 * Client (ClassA, key[64])               ->                 Server
 * Client    <-   (ClassBB, NetworkStatus, ChessboardChange) Server
 * 1.1 验证失败
 * Client (ClassA, key[64])               ->                 Server
 * Client    <-                     (ClassAA, NetworkStatus) Server
 *
 * 2. 下棋
 * Client (ClassC, key[64], Step)         ->                 Server
 * Client    <-   (ClassBB, NetworkStatus, ChessboardChange) Server
 *
 */