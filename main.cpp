#include "includes.h"

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

bool fileIsEmpty(std::ifstream& pFile);                                                     // check file - is it empty?
std::vector<std::string> getArgumentsWithoutOptions(const std::vector<std::string>& vec);   // check input for additional options and save them. Returned value - arguments from 
                                                                                                        // the command line without additional options
void printHelpMessage();                                                                    // print help - usage, possible input etc.


template <class T>
void printType(const T&)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
}


int main(int argc, char** argv)
{
    std::vector<std::string> allArguments(argv + 1, argv + argc); // save argv's to the vector of strings

    std::vector<std::string> arguments = getArgumentsWithoutOptions(allArguments);

    const int argsSize = arguments.size();
    // checking input
	if(argsSize < 2 && !PARAMETERS::bHELP)
	{
        std::cerr << CColors::RED + "Incorrect input!\n"
	              << CColors::WHITE + "Usage:\t./jsonParser [-h] [path/to/the/files]\n";
		return -1;
	}
    else if (PARAMETERS::bHELP) // if the input is like this: "./jsonParser -h" or "./jsonParser --help" or "./jsonParser fds sdg asdf -h" we print help message and exit
    {
        printHelpMessage();
        return 0;
    }
    
    std::set<std::string> allKeys;
    std::map<std::string, boost::variant<nlohmann::json, std::string, std::bitset<16>, bool, int, unsigned int, float> > mp[argsSize];

	// parse json files to string
	for(int i = 0; i < argsSize; ++i)
	{
		// open the filestream
		std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.open(arguments[i]);
		}
		catch (const std::ifstream::failure& ex)
		{
			// checking for opening files
			std::cerr << "Error while opening the file " << arguments[i];
            std::cerr << "Error code: " << ex.what() << '\n';
			return -1;
		}

		// check file - is it empty?
		if(fileIsEmpty(file))
		{
			std::cerr << "File " << arguments[i] << " is empty!\n";
			return -1;
		}

        nlohmann::json j;
        try
        {
	        j = nlohmann::json::parse(file);
        }
        catch(const std::exception& ex)
        {
            std::cout << "An error occured while parsing the file " << arguments[i] << '\n';
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
            mp[i].insert(std::make_pair(it.key(), varVal));
            allKeys.insert(it.key());
		}

		file.close();
	}

    tabulate::Table prettyTable;
    
    // firstly, we need to write the columns in one vector (one value in the vector - one column in the table), and after this put this vector to add_row()
    std::vector<std::variant<std::string, const char*, tabulate::Table>> vec;

    // set header to our table
    vec.push_back("Keys");
    for(int i = 0; i < argsSize; ++i)
    {
        vec.push_back(arguments[i]);
    }
    prettyTable.add_row(vec);

    // bCOLORIZE header

    for(int i = 0; i < argsSize + 1; ++i)
    {
        prettyTable[0][i].format()
                .font_align(tabulate::FontAlign::center)
                .font_style({tabulate::FontStyle::bold});
        if(PARAMETERS::bCOLORIZE)
        {
            prettyTable[0][i].format().font_color(tabulate::Color::yellow);
        }
    }
    
    // bool bAreDifferentValuesInMaps   = false;    // if values from the keys are different in the same key, we can paint the cell in yellow
    // bool bValueDoesntExist           = false;    // if value on such keys doesn't exists and we put '-' to the table, we can paint the cell in red

    int counter = 0;
    for (const auto& it : allKeys)
    {
        vec.clear();
        vec.push_back(it);
        // put key in to the first column in our spreadsheet
        for (int i = 0; i < argsSize; ++i)
        {
            // check - is such key exists in this map?
            if (mp[i].find(it) != mp[i].end())
            {
                // if key exists - get value and put into row (vector)
                vec.push_back(boost::apply_visitor(my_visitor(), mp[i].find(it)->second));
            }
            else
            {
                // if key doesn't contains in this map - push '-' to the cell in the table
                vec.push_back(static_cast<std::string>("-"));
            }
        }
        prettyTable.add_row(vec);

        if (PARAMETERS::bCOLORIZE)
        {
            for (int i = 0; i < argsSize + 1; ++i)
            {
                try
                {
                    // if value on such keys doesn't exists and we put '-' to the table, we can paint the cells in this row in red
                    if(std::holds_alternative<std::string>(vec[i]))
                    {
                        if ( std::get<std::string>(vec[i]) == static_cast<std::string>("-"))
                        {
                            std::cout << "i: " << i << '\n';
                            prettyTable[counter + 1][i].format().font_background_color(tabulate::Color::red);
                        }
                    }                    
                }
                catch(std::bad_variant_access const& ex)
                {
                    std::cout << ex.what() << ": vec[i] contained smth not a string\n";
                }
            }
            ++counter;

            // for(int i = 0; i < vec.size(); ++i)
            // {
            //     for (int j = i; j < vec.size(); ++j)
            //     {
            //         if ( !strcmp(vec[i], vec[i]) )
            //         {
            //             for(int k = 0; k < argsSize + 1; ++k)
            //             {
            //                 prettyTable[i][j].format().font_background_color(tabulate::Color::yellow);
            //             }
            //         }
            //     }
            // }
        }
    }

    // print table and exit
    std::cout << prettyTable << '\n';
    return 0;
}


bool fileIsEmpty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

std::vector<std::string> getArgumentsWithoutOptions(const std::vector<std::string>& vec)
{
    std::vector<std::string> returnedVector;
    for (const auto& it: vec)
    {
        if(!strcmp(it.c_str(), "-h") || !strcmp(it.c_str(), "--help"))
        {
            PARAMETERS::bHELP = true;
        }
        else if(!strcmp(it.c_str(), "-c") || !strcmp(it.c_str(), "--bCOLORIZE"))
        {
            PARAMETERS::bCOLORIZE = true;
        }
        else
        {
            returnedVector.push_back(it);
        }
    }

    return returnedVector;
}

void printHelpMessage()
{
	std::cout << "Usage:\t./jsonParser [-h] [path/to/the/files]\n\n";

    
	std::cout << "Optional arguments:\n";
	if (PARAMETERS::bCOLORIZE)
    {
        std::cout << "\t-h, --help\t\t\t" + CColors::BLUE + "Show this help message and exit\n" + CColors::WHITE;
	    std::cout << "\t-c, --bCOLORIZE\t\t\t" + CColors::BLUE + "Print the table using the colors\n" + CColors::WHITE;
    }
    else
    {
        std::cout << "\t-h, --help\t\t\tShow this help message and exit\n";
	    std::cout << "\t-c, --bCOLORIZE\t\t\tPrint the table using the colors\n";
    }
    std::cout << '\n';
	std::cout << "###################\n";
	std::cout << "#  Usage Examples #\n";
	std::cout << "###################\n\n";

	std::cout << "$ ./jsonParser -h\n";
    std::cout << "$ ./jsonParser -h -c\n";
	std::cout << "$ ./jsonParser jsonFilesForTest/first.json jsonFilesForTest/second.json\n";
    std::cout << "$ ./jsonParser -c jsonFilesForTest/file1.json jsonFilesForTest/file2.json\n";
}