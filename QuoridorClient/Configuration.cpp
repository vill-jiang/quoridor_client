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

Configuration::Configuration(NetworkClient* net) {  // ע��,�����ָ���������Ϸ�������ͷ�
    if (net == nullptr) {
        this->networkClient = new NetworkClient();  // Ĭ������127.0.0.1:5620
    } else {
        this->networkClient = net;
    }
}

Configuration::Configuration(const std::string& host, const int port) {
    this->networkClient = new NetworkClient(host, port);
}


UserStatus Configuration::getChange(ChessboardChange& newChange) const {
    int tmp = 0;  // ��С��ת���ݴ� 
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
    int tmp = 0;  // ��С��ת���ݴ� 
    MessageType type;
    if (step == nullptr) {  // ClientVerifyKey
        type = MessageType::ClientVerifyKey;
    } else {                // ClientPlayGame
        type = MessageType::ClientPlayGame;
    }
    // һ�������� 
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
    // �ֲ��������� 
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
    if (this->histories.empty()) {  // �о��ĵ�һ�� 
        if (lastChange.enemyLoc == PLAYER0_LOC || lastChange.enemyLoc == PLAYER1_LOC) {  // ���˵�һ��û�� 
            enemyLastLoc = lastChange.enemyLoc;
        } else { // ��һ�����˱�ʾ�Լ���֪��֮ǰ��λ�� 
            enemyLastLoc.x = -1;
            enemyLastLoc.y = -1;
        }
    }
    if (!lastChange.isWin()) {         // δӮ����Ϸ�ż�¼ 
        Step enemyStep(lastChange);
        if (enemyStep.myNewLoc == enemyLastLoc) {
            enemyStep.myNewLoc.x = -1;
            enemyStep.myNewLoc.y = -1;
        } else if (enemyStep.isMove()) {
            enemyLastLoc = enemyStep.myNewLoc;
        }
        this->histories.emplace_back("Enemy", enemyStep, QuoridorUtils::GameStatus::None);
        if (!lastChange.isFinish()) {  // δӮ����Ϸ��δ���(�����Լ���)���¼�Լ���һ�� 
            this->histories.emplace_back("Me", myStep, QuoridorUtils::GameStatus::None);
        } else {  // �Լ����� 
            switch (lastChange.status) {
            case GameStatus::Lost:
                if (enemyLastLoc.y == 8) {
                    enemyLastLoc.y = 9;
                } else if (enemyLastLoc.y == 2) {
                    enemyLastLoc.y = 1;
                }
                std::get<1>(this->histories.back()).myNewLoc = enemyLastLoc;
                std::get<2>(this->histories.back()) = GameStatus::Win;  // ����Ӯ�˱ض�Ϊ���嵽�յ�
                this->histories.emplace_back("Me", Step(), lastChange.status);
                break;
            case GameStatus::RulesBreaker:   // �Լ��ƻ�����������޲��� 
                this->histories.pop_back();
                break;
            default: ;
            }
        }
    } // Ӯ�˵�����Ϊ�������¼ 
    if (!this->histories.empty()) {    // ��¼��һ��״̬ 
        std::get<2>(this->histories.back()) = lastChange.status;
    }
    //lastStatus = lastChange.status;
}

void Configuration::startGame(Player& player, std::vector<std::string>& logFiles, const bool logSave) {
    // ���key 
    this->playerKey = player.getKey();
    player.restart();
    ChessboardChange chessboard;
    UserStatus userStatus = transDataAndGetChange(chessboard);
    if (userStatus == UserStatus::KeyError) {
        // ����key�ȴ��ظ����ظ�ȷ����ʼ��Ϸ������ر������˳� 
        return ;
    } else {
        std::cout << "Waiting for start." << std::endl;
        // ��ʼ��Ϸѭ������ player.nextStep()��ֱ����Ϸ���� 
        while (userStatus == UserStatus::Ok) {           // ÿ�ζ���Ҫ���key��֤�Ƿ�ɹ� 
            if (chessboard.isFinish()) {                 // ÿ�ζ���Ҫ�����Ϸ�Ƿ���� 
                record(chessboard);                      // ��¼���һ�� 
                std::cout << "Game result:" << chessboard.status << std::endl;
                if (logSave) {
                    logFiles.push_back(saveGameHistory());  // �����ļ�
                    std::cout << "Save log file to " << logFiles.back() << std::endl;
                }
                this->histories.clear();
                player.restart();
                std::cout << "Clean your player." << std::endl;
                userStatus = getChange(chessboard);
                continue;
            }
            Step newStep = player.nextStep(chessboard);  // ����
            // todo: ��ʱ�����Ҿ���(���߳�) 
            record(chessboard, newStep);                 // ��¼���ֲ�����Լ�����, ���ֲ����е�״̬Ϊ��һ���Լ����ߵ�״̬ 
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
