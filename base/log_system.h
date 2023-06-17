#pragma once

#include <iostream>

//TODO change thie to spdlog and add output logfile

namespace Log
{
    static void info(const std::string& info){
        std::cout << "[info]: " << info.c_str() << std::endl;
    }

    static void error(const std::string& error){
        std::cerr << "[error]: " << error.c_str() << std::endl;
    }

}