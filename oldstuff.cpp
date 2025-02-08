

// en dålig funktion för parsing av datan. 
// sscanf gjorde det bättre
Object parse_line(const std::string& line)
{
    std::unordered_map<std::string, std::string> key_value_map;

    // splittar string på ; (ungefär, läser egentligen en del av string i taget) 
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ';'))
    {
        size_t pos = token.find('='); //ta fram position för =
        if (pos != std::string::npos)
        {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            key_value_map[key] = value;
        }
    }

    int64_t id = std::stoll(key_value_map["ID"]);
    int32_t x = std::stoi(key_value_map["X"]);
    int32_t y = std::stoi(key_value_map["Y"]);
    uint8_t type = static_cast<uint8_t>(std::stoi(key_value_map["TYPE"]));


    std::cout << "object created" << std::endl;

    // skapar skiten
    return Object(id, x, y, type);
}