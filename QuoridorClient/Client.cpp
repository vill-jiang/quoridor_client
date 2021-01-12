#include <iostream>
#include <fstream>
#include <thread>
#include "NetworkClient.h"
#include "Configuration.h"
#include "MyPlayer.h"
#include "BaselinePlayer.h"
#include "DefensivePlayer.h"
#include "DefensiveRunPlayer.h"
//#include "LogPlayer.h"
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>

const std::string robotName[5] = { "MyPlayer", "Defensive", "Run", "DefensiveRun", "Baseline" };

bool getConfig(int argc, char* argv[], nlohmann::json& config);
void playGame(const nlohmann::json& config, QuoridorUtils::Player& player, 
              std::vector<std::string>& logFiles, bool saveLogFile = true);
void reviewGame(QuoridorUtils::GameStatus status, const std::string& cmd = "", const std::string& logFilename = "");

int main(int argc, char* argv[]) {
    // 1. 读配置文件, 默认配置文件为当前目录下的config.json 
    // 可以输入参数读取其他的配置文件, 详见 getConfig() 函数 
    nlohmann::json config;
    if (!getConfig(argc, argv, config)) {
        return 0;
    }
    // 2. 新建玩家并开始游戏 
    std::vector<std::string> logFiles;
    const auto threads = config.value("threads", 0);
    const auto threadsMultiPorts = config.value("threadsMultiPorts", false);
    const auto robotNumber = config.value("robot", 4);
    QuoridorUtils::Player* myPlayer;
    if (threads <= 1) {
        // 单玩家
        switch (robotNumber) {
        case 0:
            myPlayer = new QuoridorUtils::MyPlayer(config.value("playerKey", ""));
            break;
        case 1:
            myPlayer = new QuoridorUtils::DefensivePlayer(config.value("playerKey", ""));
            break;
        case 2:
            myPlayer = new QuoridorUtils::RunPlayer(config.value("playerKey", ""));
            break;
        case 3:
            myPlayer = new QuoridorUtils::DefensiveRunPlayer(config.value("playerKey", ""));
            break;
        case 4:
        default:
            myPlayer = new QuoridorUtils::BaselinePlayer(config.value("playerKey", ""));
            break;
        }
        std::cout << "Build robot: " << robotName[robotNumber] << std::endl;
        playGame(config, *myPlayer, logFiles, config.value("saveLog", true));
        delete(myPlayer);
    } else {
        // 多玩家 
        const int threadsCount = config.value("threads", 2);
        std::list<std::thread*> gameThreads;
        std::list<QuoridorUtils::Player*> gamePlayers;
        std::list<nlohmann::json*> threadConfigs;
        const int portStart = config["network"]["port"];
        for (auto i = 0; i < threadsCount; i++) {
            nlohmann::json* threadConfig = nullptr;
            if (threadsMultiPorts) {
                threadConfig = new nlohmann::json(config);
                (*threadConfig)["network"]["port"] = portStart + i;
                threadConfigs.emplace_back(threadConfig);
            } else {
                threadConfig = &config;
            }
            switch (robotNumber) {
            case 0:
                myPlayer = new QuoridorUtils::MyPlayer(config.value("playerKey", ""));
                break;
            case 1:
                myPlayer = new QuoridorUtils::DefensivePlayer(config.value("playerKey", ""));
                break;
            case 2:
                myPlayer = new QuoridorUtils::RunPlayer(config.value("playerKey", ""));
                break;
            case 3:
                myPlayer = new QuoridorUtils::DefensiveRunPlayer(config.value("playerKey", ""));
                break;
            case 4:
            default:
                myPlayer = new QuoridorUtils::BaselinePlayer(config.value("playerKey", ""));
                break;
            }
            std::cout << "Build robot: " << robotName[robotNumber] << std::endl;
            auto t = new std::thread(playGame, std::ref(*threadConfig), std::ref(*myPlayer),
                                     std::ref(logFiles), (*threadConfig).value("saveLog", false));
            gameThreads.push_back(t);
            gamePlayers.push_back(myPlayer);
        }
        for (auto t : gameThreads) {
            (*t).join();
            delete(t);
        }
        for (auto player : gamePlayers) {
            delete(player);
        }
        if (threadsMultiPorts) {
            for (auto threadConfig : threadConfigs) {
                delete(threadConfig);
            }
        }
    }
    // 3. 游戏回放 
    if (config.value("gameReplay", false)) {
        for (auto logFile : logFiles) {
            reviewGame(QuoridorUtils::GameStatus::Ok, config.value("gameReplayCmd", "./QuoridorUI  file%"),
                       logFile);
        }
    }
    return 0;
}

void playGame(const nlohmann::json& config, QuoridorUtils::Player& player, 
              std::vector<std::string>& logFiles, const bool saveLogFile) {
    // 2.1. 连接网络, 默认连接 127.0.0.1:5620 
    NetworkClient* network = nullptr;
    if (config.contains("network")) {
        const auto networkConfig = config.value("network", "{ \"host\": \"127.0.0.1\", \"port\": 5620 }"_json);
        network = new NetworkClient(networkConfig.value("host", "127.0.0.1"),
                                    networkConfig.value("port", 5620));
    } else {
        network = new NetworkClient();
    }
    if (network->getStatus() != NetworkStatus::Ok) {
        std::cout << "Your network connect error! pls check your network." << std::endl;
        delete(network);
        return ;
    }
    // 2.2. 开始游戏 
    QuoridorUtils::Configuration quoridor(network);
    quoridor.startGame(player, logFiles, saveLogFile);
    // 2.3. 游戏结果输出并关闭网络 
    delete(network);
}

void reviewGame(const QuoridorUtils::GameStatus status, const std::string& cmd, const std::string& logFilename) {
    if (status != QuoridorUtils::GameStatus::None) {
        //std::string logFilename = quoridor.saveGameHistory();
        if (!logFilename.empty() && !cmd.empty()) { // config.value("gameReplay", false)
            std::string cmdTmpStr = cmd;  //
            size_t pos = cmdTmpStr.find("%file%");
            cmdTmpStr.replace(pos, 6, logFilename);
#if defined(_WIN32) || defined(_WIN64)
            do {
                pos = cmdTmpStr.find('/');
                if (pos != std::string::npos) {
                    cmdTmpStr.replace(pos, 1, "\\");
                }
            } while (pos == std::string::npos);
#endif
            system(cmdTmpStr.data());
        }
    }
}

bool getConfig(int argc, char* argv[], nlohmann::json& config) {
    try {
        cxxopts::Options options("QuoridorClient", "Quoridor game client. Author: Jiang HuiYu.");
        options.add_options()
            ("c,config", "Config file", cxxopts::value<std::string>())
            ("h,host", "Host ip", cxxopts::value<std::string>()->default_value("127.0.0.1"))  //127.0.0.1
            ("p,port", "Port number", cxxopts::value<unsigned int>()->default_value("19330"))
            ("t,threads", "Threads count, 0 or 1 for close multiple threads", cxxopts::value<unsigned int>())
            ("m,multiPorts", "Enable thread multiple port", cxxopts::value<bool>())
            ("k,key", "Player key, not for robot", cxxopts::value<std::string>()->default_value("19120000"))
            ("l,log", "Enable save log", cxxopts::value<bool>()->default_value("false"))
            ("r,replay", "Enable game replay", cxxopts::value<bool>()->default_value("false"))
            ("C,replayCmd", "Game replay Command", cxxopts::value<std::string>()->default_value("./QuoridorUI %file%"))
            ("R,robot", "Choose robot [0.MyPlayer, 1.Defensive, 2.Run, 3.DefensiveRun, 4.Baseline]", cxxopts::value<unsigned int>()->default_value("0"))
            ("help", "Print usage")
            ;
        const auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return false;
        }
        unsigned int threads = 0;
        auto threadsMultiPorts = false;
        if (result.count("threads")) {
            threads = result["threads"].as<unsigned int>();
            if (result.count("multiPorts")) {
                threadsMultiPorts = result["multiPorts"].as<bool>();
            }
        }
        auto robot = result["robot"].as<unsigned int>();
        auto host = result["host"].as<std::string>();
        auto port = result["port"].as<unsigned int>();
        auto key = result["key"].as<std::string>();
        auto log = result["log"].as<bool>();
        auto replay = result["replay"].as<bool>();
        auto replayCmd = result["replayCmd"].as<std::string>();
        if (result.count("config")) {
            const auto configFilename = result["config"].as<std::string>();
            std::ifstream configFile(configFilename);
            if (configFile.is_open()) {
                configFile >> config;
                configFile.close();
                if (config.contains("network")) {
                    if (!config["network"].contains("host")) {
                        std::cerr << "[\"network\"][\"host\"]" << " don't exist in " << configFilename << "! Replayer with "
                            << host << std::endl;
                        config["network"]["host"] = host;
                    }
                    if (!config["network"].contains("port")) {
                        std::cerr << "[\"network\"][\"port\"]" << " don't exist in " << configFilename << "! Replayer with "
                            << port << std::endl;
                        config["network"]["host"] = port;
                    }
                } else {
                    std::cerr << "[\"network\"]" << " don't exist in " << configFilename << "!" << std::endl;
                    config["network"]["host"] = host;
                    config["network"]["port"] = port;
                }
                if (!config.contains("playerKey")) {
                    std::cerr << "[\"playerKey\"]" << " don't exist in " << configFilename << "! Replayer with "
                        << key << std::endl;
                    config["playerKey"] = key;
                }
                if (!config.contains("saveLog")) {
                    std::cerr << "[\"saveLog\"]" << " don't exist in " << configFilename << "! Replayer with "
                        << log << std::endl;
                    config["saveLog"] = log;
                }
                if (!config.contains("gameReplay")) {
                    std::cerr << "[\"gameReplay\"]" << " don't exist in " << configFilename << "! Replayer with "
                        << replay << std::endl;
                    config["gameReplay"] = replay;
                }
                if (!config.contains("gameReplayCmd")) {
                    std::cerr << "[\"gameReplayCmd\"]" << " don't exist in " << configFilename << "! Replayer with "
                        << replayCmd << std::endl;
                    config["gameReplayCmd"] = replayCmd;
                }
                if (!config.contains("robot")) {
                    std::cerr << "[\"robot\"]" << " don't exist in " << configFilename << "! Replayer with "
                        << robot << std::endl;
                    config["robot"] = robot;
                }
                if (!config.contains("threads")) {
                    std::cerr << "[\"threads\"]" << " don't exist in " << configFilename << "! Replayer with "
                        << threads << std::endl;
                    if (threads > 0) {
                        config["threads"] = threads;
                        config["threadsMultiPorts"] = threadsMultiPorts;
                    } else {
                        config["threads"] = 0;
                        config["threadsMultiPorts"] = false;
                    }
                }
                return true;
            } else {
                std::cerr << configFilename << " don't exist!" << std::endl;
                return false;
            }
        } else {
            config["network"]["host"] = host;
            config["network"]["port"] = port;
            config["playerKey"] = key;
            config["saveLog"] = log;
            config["gameReplay"] = replay;
            config["gameReplayCmd"] = replayCmd;
            config["robot"] = robot;
            if (threads > 0) {
                config["threads"] = threads;
                config["threadsMultiPorts"] = threadsMultiPorts;
            } else {
                config["threads"] = 0;
                config["threadsMultiPorts"] = false;
            }
        }
        return true;
    } catch (const cxxopts::OptionException& e) {
        std::cout << e.what() << std::endl;
        exit(-1);
    }
}
