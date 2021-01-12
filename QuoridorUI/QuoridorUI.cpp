#include <iostream>
#include <fstream>
#include "ConsoleView.h"
#include "LogLoader.h"
#include "Replayer.h"
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>

bool getConfig(int argc, char* argv[], nlohmann::json& config);

int main(int argc, char* argv[]) {
    nlohmann::json config;
    if (!getConfig(argc, argv, config)) {
        return 0;
    }
    QuoridorUtils::ConsoleView view(config["displayConfig"]);
    QuoridorUtils::LogLoader logLoader(config["logFile"]);
    QuoridorUtils::Replayer replayer;
    replayer.start(view, logLoader);
    return 0;
}

bool getConfig(int argc, char* argv[], nlohmann::json& config) {
    try {
        cxxopts::Options options("QuoridorUI", "Quoridor game user interface. Author: Jiang HuiYu.");
        options.add_options()
            ("f,file", ".csv for log file, .json for config file", 
             cxxopts::value<std::string>()->default_value("uiconfig.json"))
            ("c,config", "Config file name (default: uiconfig.json)", cxxopts::value<std::string>())
            ("l,log", "Log file name", cxxopts::value<std::string>())
            ("h,help", "Print usage")
            ;
        options.parse_positional("file");
        const auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return false;
        }

        auto file = result["file"].as<std::string>();
//        std::string exeDir;
//        if (result.arguments().empty()) {
//            exeDir = std::string(argv[0]);
//#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
//            auto diff = exeDir.find_last_of('\\');
//#elif defined(__APPLE__) || defined(LINUX) || defined(linux) || defined(__unix__)
//            auto diff = exePath.find_last_of('/');
//#endif
//            exeDir.erase(diff + 1);
//            file = exeDir + file;
//        }
        std::string configFilename;
        if (result.count("config")) {
            configFilename = result["config"].as<std::string>();
        } else if (file.length() > 5 && file.substr(file.length() - 5, 5) == ".json") {
            configFilename = file;
        } else {
            configFilename.assign("uiconfig.json");
        }
        std::string logFilename;
        if (result.count("log")) {
            logFilename = result["log"].as<std::string>();
        } else if (file.length() > 4 && file.substr(file.length() - 4, 4) == ".csv") {
            logFilename = file;
        }

        std::ifstream configFile(configFilename);
        if (configFile.is_open()) {
            configFile >> config;
            configFile.close();
        } else {
            std::cerr << configFilename << " don't exist!" << std::endl;
            return false;
        }
        if (!logFilename.empty()) {
            config["logFile"] = logFilename;
        }
        return true;
    } catch (const cxxopts::OptionException& e) {
        std::cout << e.what() << std::endl;
        exit(-1);
    }
}
