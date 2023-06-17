#pragma once


#include <exception>
#include <string>

#include "base/utils.h"


// class FrameException: public std::exception{

// public:
//     FrameExcepthin(const std::string& msg, ...){

        
//         char msg[1000];
//         va_list args;
//         va_start(args, msg);
//         VSNPRINTF(msg, sizeof(msg), msg, args);
//         va_end(args);

//         char msg2[1000];
//         SNPRINTF(msg2, sizeof(msg2), "%s:%d: %s", __FILE__, __LINE__, msg);

//         std::exception(msg2);
//     }
// };