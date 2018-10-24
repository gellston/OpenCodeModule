#pragma once


#include <string>

#define OCERROR(name) std::string name = ""; name += __FILE__; name += ":" + std::to_string(__LINE__); name += ":";
#define OCERRORWRITE(name,message) name += message; name += '\n';