// Copyright 2021 Kushpelev Alexei leha.kushpelev@mail.ru

#include "HashGenerator.hpp"

const char EndHash[] = "0000";
const size_t EndLength = 60;
std::mutex mut;
std::atomic<bool> hashGeneratorWork = true;
std::atomic<bool> existenceJson = false;

void HashGenerator::InitLogs() const {
    boost::log::add_common_attributes();
    auto TraceLogFile = boost::log::add_file_log(
            boost::log::keywords::file_name =
                    "/home/baha/Alex-kku/workspace/projects/C++/lab-06-multithreads/Logs/LogTrace_%N.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::format =
                    "[%TimeStamp%][%Severity%][%ThreadID%]: [%Message%]");
    TraceLogFile->set_filter(boost::log::trivial::severity ==
                             boost::log::trivial::trace);
    auto InfoLogFile = boost::log::add_file_log(
            boost::log::keywords::file_name =
                    "/home/baha/Alex-kku/workspace/projects/C++/lab-06-multithreads/Logs/LogInfo_%N.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::format =
                    "[%TimeStamp%][%Severity%][%ThreadID%]: [%Message%]");
    InfoLogFile->set_filter(boost::log::trivial::severity ==
                            boost::log::trivial::info);
    auto LogConsole = boost::log::add_console_log(
            std::cout, boost::log::keywords::format =
                    "[%TimeStamp%][%Severity%][%ThreadID%]: [%Message%]");
    LogConsole->set_filter(boost::log::trivial::severity >=
                           boost::log::trivial::trace);
}

void HashGenerator::SaveHash(const std::string &data, const std::string &hash, const std::time_t &timestamp) {
    std::stringstream ss;
    ss << std::uppercase << std::hex << std::stoi(data);
    nlohmann::json js;
    js["timestamp"] = timestamp;
    js["hash"] = hash;
    js["data"] = ss.str();
    std::scoped_lock<std::mutex> lock(mut);
    json.push_back(js);
}

void Stop([[maybe_unused]] int dummy) {
    hashGeneratorWork = false;
}

void HashGenerator::CalculationHash() {
    while (hashGeneratorWork) {
        std::string data = std::to_string(std::rand());
        std::string hash = picosha2::hash256_hex_string(data);
        std::time_t timestamp(std::time(nullptr));
        std::string hashEnd = hash.substr(EndLength);
        if (hashEnd == EndHash) {
            BOOST_LOG_TRIVIAL(info) << "Hash:" << hash << " Data:" << data;
            if (existenceJson) {
                SaveHash(data, hash, timestamp);
            }
        } else {
            BOOST_LOG_TRIVIAL(trace) << "Hash:" << hash << " Data:" << data;
        }
    }
}

void HashGenerator::FindHash(int &argc, char *argv[]) {
    size_t threadsNumber;
    std::string path;
    switch (argc) {
        case 1:
            threadsNumber = std::thread::hardware_concurrency();
            break;
        case 2:
            threadsNumber = std::atoi(argv[1]);
            if (threadsNumber < 1 || threadsNumber > std::thread::hardware_concurrency()) {
                throw std::out_of_range("Uncorect number threads!");
            }
            break;
        case 3:
            threadsNumber = std::atoi(argv[1]);
            if (threadsNumber < 1 || threadsNumber > std::thread::hardware_concurrency()) {
                throw std::out_of_range("Uncorect number threads!");
            }
            path = argv[2];
            existenceJson = true;
            break;
        default:
            throw std::out_of_range("Invalid arguments!");
    }
    std::signal(SIGINT, Stop);
    InitLogs();
    std::vector<std::thread> threads;
    threads.reserve(threadsNumber);
    for (size_t i = 0; i < threadsNumber; ++i) {
        threads.emplace_back(&HashGenerator::CalculationHash, this);
    }
    for (auto &thread : threads) {
        thread.join();
    }
    if (existenceJson) {
        std::ofstream out{path};
        out << json.dump(4);;
    }
}
