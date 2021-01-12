#include "Configuration.h"
#include <ctime>
#include <chrono>
#include <fstream>
#include <cstring>
#include <iostream>

namespace QuoridorUtils {

enum class MessageType {
    ClientVerifyKey = 1,     // 1 0x01000000
    ClientPlayGame = 2,      // 2 0x02000000
    ServerNoneData = 1001,   // 1001 0xe9030000
    ServerNewChange = 1002,  // 1002 0xea030000
    None = 2000,
};

enum class UserStatus {
    Ok = 0, 
    KeyError = 1, 
    Close = -1, 
    None = 2000, 
};

int Configuration::messageLength(const MessageType type) const {
    switch (type) {
    case MessageType::ClientVerifyKey:
        return sizeof(MessageType) + this->playerKey.length();  // ClientMessageType, key  68
    case MessageType::ClientPlayGame:
        return sizeof(MessageType) + this->playerKey.length() + sizeof(Step);  // ClientMessageType, key, step 92
    case MessageType::ServerNoneData:
        return sizeof(MessageType) + sizeof(GameStatus);  // ServerMessageType, status 8
    case MessageType::ServerNewChange:
        return sizeof(MessageType) + sizeof(GameStatus) + sizeof(ChessboardChange);  // ServerMessageType, status, newChange 44
    default:
        return 100;
    }
}

Configuration::Configuration(NetworkClient* net) {  // 注意,传入的指针必须在游戏结束后释放
    if (net == nullptr) {
        this->networkClient = new NetworkClient();  // 默认连接127.0.0.1:5620
    } else {
        this->networkClient = net;
    }
}

Configuration::Configuration(const std::string& host, const int port) {
    this->networkClient = new NetworkClient(host, port);
}


UserStatus Configuration::getChange(ChessboardChange& newChange) const {
    int tmp = 0;  // 大小端转换暂存 
    MessageType returnType = MessageType::None;
    UserStatus returnStatus = UserStatus::None;
    if (this->networkClient->receiveData(&tmp, sizeof(MessageType)) != NetworkStatus::Ok) {
        return UserStatus::None;
    }
    returnType = MessageType(NetworkClient::changeEndianBytes(tmp));
    if (this->networkClient->receiveData(&tmp, sizeof(UserStatus)) != NetworkStatus::Ok) {
        return UserStatus::None;
    }
    returnStatus = UserStatus(NetworkClient::changeEndianBytes(tmp));
    if (returnType == MessageType::ServerNewChange) {
        if (this->networkClient->receiveData(&newChange, sizeof(ChessboardChange)) != NetworkStatus::Ok) {
            return UserStatus::None;
        }
        newChange = changeEndian(newChange, NetworkClient::changeEndianBytes);
    }
    return returnStatus;
}

UserStatus Configuration::transDataAndGetChange(ChessboardChange& newChange, Step* step) const {
    int tmp = 0;  // 大小端转换暂存 
    MessageType type;
    if (step == nullptr) {  // ClientVerifyKey
        type = MessageType::ClientVerifyKey;
    } else {                // ClientPlayGame
        type = MessageType::ClientPlayGame;
    }
    // 一起发送数据 
    void* data = malloc(messageLength(type));
    if (data == nullptr) {
        exit(-1);
    }
    tmp = NetworkClient::changeEndianBytes(int(type));
    std::memcpy(data, &tmp, sizeof(MessageType));
    std::memcpy(static_cast<char*>(data) + sizeof(int), 
                this->playerKey.data(), sizeof(char) * this->playerKey.length());
    if (type == MessageType::ClientPlayGame) {
        Step stepTmp = changeEndian(*step, NetworkClient::changeEndianBytes);
        std::memcpy(static_cast<char*>(data) + sizeof(int) + sizeof(char) * this->playerKey.length(), 
                    &stepTmp, sizeof(Step));
    }
    if (this->networkClient->transData(data, messageLength(type)) != NetworkStatus::Ok) {
        return UserStatus::None;
    }
    std::free(data);
    // 分步发送数据 
    //this->networkClient->transData(&type, sizeof(MessageType));
    //this->networkClient->transData(this->playerKey.data(), sizeof(char) * this->playerKey.length());
    //if (type == MessageType::ClientPlayGame) {
    //    this->networkClient->transData(step, sizeof(Step));
    //}
    const auto returnStatus = getChange(newChange);
    return returnStatus;
}

void Configuration::record(const ChessboardChange& lastChange, const Step& myStep) {
    if (lastChange.isWin()) {
        this->victoryRoundCount++;
    } else if (lastChange.isLost()) {
        this->defeatedRoundCount++;
    }
    //static QuoridorUtils::GameStatus lastStatus;
    static QuoridorUtils::Location enemyLastLoc;
    if (this->histories.empty()) {  // 敌军的第一步 
        if (lastChange.enemyLoc == PLAYER0_LOC || lastChange.enemyLoc == PLAYER1_LOC) {  // 敌人第一步没动 
            enemyLastLoc = lastChange.enemyLoc;
        } else { // 第一步动了表示自己不知道之前的位置 
            enemyLastLoc.x = -1;
            enemyLastLoc.y = -1;
        }
    }
    if (!lastChange.isWin()) {         // 未赢得游戏才记录 
        Step enemyStep(lastChange);
        if (enemyStep.myNewLoc == enemyLastLoc) {
            enemyStep.myNewLoc.x = -1;
            enemyStep.myNewLoc.y = -1;
        } else if (enemyStep.isMove()) {
            enemyLastLoc = enemyStep.myNewLoc;
        }
        this->histories.emplace_back("Enemy", enemyStep, QuoridorUtils::GameStatus::None);
        if (!lastChange.isFinish()) {  // 未赢得游戏且未完成(否则自己输)则记录自己的一步 
            this->histories.emplace_back("Me", myStep, QuoridorUtils::GameStatus::None);
        } else {  // 自己输了 
            switch (lastChange.status) {
            case GameStatus::Lost:
                if (enemyLastLoc.y == 8) {
                    enemyLastLoc.y = 9;
                } else if (enemyLastLoc.y == 2) {
                    enemyLastLoc.y = 1;
                }
                std::get<1>(this->histories.back()).myNewLoc = enemyLastLoc;
                std::get<2>(this->histories.back()) = GameStatus::Win;  // 对手赢了必定为走棋到终点
                this->histories.emplace_back("Me", Step(), lastChange.status);
                break;
            case GameStatus::RulesBreaker:   // 自己破坏规则则对手无操作 
                this->histories.pop_back();
                break;
            default: ;
            }
        }
    } // 赢了的数据为空无需记录 
    if (!this->histories.empty()) {    // 记录上一个状态 
        std::get<2>(this->histories.back()) = lastChange.status;
    }
    //lastStatus = lastChange.status;
}

void Configuration::startGame(Player& player, std::vector<std::string>& logFiles, const bool logSave) {
    // 获得key 
    this->playerKey = player.getKey();
    player.restart();
    ChessboardChange chessboard;
    UserStatus userStatus = transDataAndGetChange(chessboard);
    if (userStatus == UserStatus::KeyError) {
        // 发送key等待回复，回复确认则开始游戏，否则关闭连接退出 
        return ;
    } else {
        std::cout << "Waiting for start." << std::endl;
        // 开始游戏循环调用 player.nextStep()，直至游戏结束 
        while (userStatus == UserStatus::Ok) {           // 每次都需要检测key验证是否成功 
            if (chessboard.isFinish()) {                 // 每次都需要检测游戏是否结束 
                record(chessboard);                      // 记录最后一步 
                std::cout << "Game result:" << chessboard.status << std::endl;
                if (logSave) {
                    logFiles.push_back(saveGameHistory());  // 保存文件
                    std::cout << "Save log file to " << logFiles.back() << std::endl;
                }
                this->histories.clear();
                player.restart();
                std::cout << "Clean your player." << std::endl;
                userStatus = getChange(chessboard);
                continue;
            }
            Step newStep = player.nextStep(chessboard);  // 决策
            // todo: 超时打断玩家决策(多线程) 
            record(chessboard, newStep);                 // 记录对手步骤和自己决策, 对手步骤中的状态为上一步自己决策的状态 
            userStatus = transDataAndGetChange(chessboard, &newStep);
        }
        switch (userStatus) {
        case UserStatus::Ok: break;
        case UserStatus::KeyError:
            std::cout << "Your key error, pls check your key. " << std::endl;
            break;
        case UserStatus::Close:
        case UserStatus::None:
            std::cout << "All done. " << std::endl;
            break;
        default:
            std::cout << "Undefined error. " << std::endl;
        }
        std::cout << "result: victory(" << this->victoryRoundCount
                  << "), defeated(" << this->defeatedRoundCount << ")." << std::endl;
    }
}

std::string Configuration::saveGameHistory() const {
    //auto nowTime = std::chrono::system_clock::now();
    //auto now = std::chrono::system_clock::to_time_t(nowTime);
    auto now = time(nullptr);
    const auto ltm = std::localtime(&now);
    if (ltm == nullptr) {
        return "";
    } else {
        char buf[30] = { '\0' };
        std::ifstream fTest;
        bool flag = false;
        do {
            const auto d = std::chrono::system_clock::now().time_since_epoch();
            const int nanosecond = (std::chrono::nanoseconds(d).count() / 100) % 1000;
            sprintf(buf, "log_%04d%02d%02d_%02d%02d%02d_%03d.csv",
                    ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday,
                    ltm->tm_hour, ltm->tm_min, ltm->tm_sec, nanosecond);
            fTest.open(buf);
            flag = fTest.is_open();
            if (flag) {
                fTest.close();
            }
        } while (flag);
        const std::string filename(buf);
        std::ofstream out(filename);
        if (!out.is_open()) {
            return "";
        }
        out << "player,newLoc_x,newLoc_y,newBar_start_x,newBar_start_y,newBar_stop_x,newBar_stop_y,status" << std::endl;
        for (const auto& element : this->histories) {
            std::string roleName;
            QuoridorUtils::Step newStep;
            QuoridorUtils::GameStatus stepStatus;
            std::tie(roleName, newStep, stepStatus) = element;
            out << roleName << ',' << newStep.myNewLoc.x << ',' << newStep.myNewLoc.y << ','
                << newStep.myNewBlockBar.start.x << ',' << newStep.myNewBlockBar.start.y << ','
                << newStep.myNewBlockBar.stop.x << ',' << newStep.myNewBlockBar.stop.y << ','
                << int(stepStatus) << std::endl;
        }
        out.close();
        return filename;
    }
}

Configuration::~Configuration() {
    //delete(this->networkClient);
}
}
