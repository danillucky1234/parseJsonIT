#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <boost/variant.hpp>
#include <sstream>
#include <array>
#include <bitset>
#include <boost/sort/spreadsort/string_sort.hpp>
#include "json/single_include/nlohmann/json.hpp"
#include "tabulate/single_include/tabulate/tabulate.hpp"

// CONST GLOBAL VARIABLES
const std::string RED_COLOR_TEXT	= "\033[31m"; 
const std::string GREEN_COLOR_TEXT	= "\033[32m"; 
const std::string BLUE_COLOR_TEXT	= "\033[36m"; 
const std::string WHITE_COLOR_TEXT	= "\033[0m";  

namespace PARAMETERS
{
    bool COLORIZE       = false;
    bool HELP           = false;

}