// Copyright 2021 Kushpelev Alexei leha.kushpelev@mail.ru

#ifndef INCLUDE_HASHGENERATOR_HPP_
#define INCLUDE_HASHGENERATOR_HPP_

#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <nlohmann/json.hpp>
#include "picosha2.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <mutex>
#include <ctime>
#include <thread>

class HashGenerator {
public:
    HashGenerator() = default;

    void FindHash(int &argc, char *argv[]);

private:
    void InitLogs() const;

    void CalculationHash();

    void SaveHash(const std::string &data, const std::string &hash, const std::time_t &timestamp);

    nlohmann::json json;
};

#endif //INCLUDE_HASHGENERATOR_HPP_
