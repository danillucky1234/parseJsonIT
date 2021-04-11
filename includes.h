#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <boost/variant.hpp>
#include <sstream>
#include <array>
#include <bitset>
#include <regex>
#include <boost/sort/spreadsort/string_sort.hpp>
#include "json/single_include/nlohmann/json.hpp"
#include "tabulate/single_include/tabulate/tabulate.hpp"

// Custom colors namespace
namespace CColors
{
    const std::string RED	= "\033[31m"; 
    const std::string GREEN	= "\033[32m"; 
    const std::string BLUE	= "\033[36m"; 
    const std::string WHITE	= "\033[0m";  
}

// namespace with possible options
namespace PARAMETERS
{
    bool bCOLORIZE          = false;
    bool bHELP              = false;
    bool bPath              = false;
}