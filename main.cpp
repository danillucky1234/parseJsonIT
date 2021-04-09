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

class my_visitor: public boost::static_visitor<std::string>
{
public:
    std::string operator() (double d) const
    {
        std::ostringstream ss;
        ss << d;
        return ss.str();
    };

    std::string operator() (std::string s) const
    {
        return s;
    };

    std::string operator() (std::bitset<16> b) const
    {
        return b.to_string(); 
    };

    std::string operator() (const char* s) const 
    {
        return s;
    };

    std::string operator() (bool b) const
    {
        return b ? "true" : "false";
    };

    std::string operator() (int i) const
    {
        std::ostringstream ss;
        ss << i;
        return ss.str();
    };

    std::string operator() (unsigned int ui) const
    {
        std::ostringstream ss;
        ss << ui;
        return ss.str();
    };

    std::string operator() (float f) const
    {
        std::ostringstream ss;
        ss << f;
        return ss.str();
    };

    std::string operator() (nlohmann::json j) const 
    {
        std::ostringstream ss;
        ss << j;
        return ss.str();
    };

};

bool fileIsEmpty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

int main(int argc, char** argv)
{
    // checking input
	if(argc < 3)
	{
		std::cerr << "Not enough files!\n";
		return -1;
	}
    
    std::set<std::string> allKeys;
    std::map<std::string, boost::variant<nlohmann::json, std::string, std::bitset<16>, bool, int, unsigned int, float> > mp[argc - 1];

	// parse json files to string
	for(int i = 1; i < argc; ++i)
	{
		// open the filestream
		std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
            
			file.open(argv[i]);
		}
		catch (const std::ifstream::failure& ex)
		{
			// checking for opening files
			std::cerr << "Error while opening the file " << argv[i] << ":\t" << ex.what() << '\n';
			return -1;
		}

		// check file - is it empty?
		if(fileIsEmpty(file))
		{
			std::cerr << "File " << argv[i] << " is empty!\n";
			return -1;
		}

        nlohmann::json j;
        try
        {
	        j = nlohmann::json::parse(file);
        }
        catch(const std::exception& ex)
        {
            std::cout << "An error occured while parsing the file " << argv[i] << '\n';
            return -1;
        }

        // it.vaue() can return one of this types, so we use variant
        boost::variant<nlohmann::json, std::string, std::bitset<16>, bool, int, unsigned int, float> varVal;

		// get keys and values and save them into map:
		//					file1.json - map[0][...]
		//					file2.json - map[1][...]
		//					file3.json - map[2][...]
		//							...
		for (const auto& it : j.items())
		{
            varVal = it.value();
            mp[i - 1].insert(std::make_pair(it.key(), varVal));
            allKeys.insert(it.key());
		}

		file.close();
	}

    tabulate::Table prettyTable;
    
    // firstly, we need to write the columns in one vector (one value in the vector - one column in the table), and after this put this vector to add_row()
    std::vector<std::variant<std::string, const char*, tabulate::Table>> vec;

    // set header to our table
    vec.push_back("Keys");
    for(int i = 1; i < argc; ++i)
    {
        vec.push_back(argv[i]);
    }
    prettyTable.add_row(vec);

    // colorize header 
    for(int i = 0; i < argc; ++i)
    {
        prettyTable[0][i].format()
                .font_color(tabulate::Color::yellow)
                .font_align(tabulate::FontAlign::center)
                .font_style({tabulate::FontStyle::bold});
    }

    for (const auto& it : allKeys)
    {
        vec.clear();
        vec.push_back(it);
        // put key in to the first column in our spreadsheet
        for (int i = 0; i < argc - 1; ++i)
        {
            // check - does such key exists in this map?
            if (mp[i].find(it) != mp[i].end())
            {
                // if key exists - get value and put into row (vector)
                vec.push_back(boost::apply_visitor(my_visitor(), mp[i].find(it)->second));
            }
            else
            {
                // if key doesn't contains in this map - push '-' to the cell in the table
                vec.push_back("-");
            }
        }
        prettyTable.add_row(vec);
    }

    // print table and exit
    std::cout << prettyTable << '\n';
    return 0;
}



