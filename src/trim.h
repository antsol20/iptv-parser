#ifndef TRIM_H
#define TRIM_H



inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

template<typename InputIt, typename T = typename std::iterator_traits<InputIt>::value_type>
std::vector<std::vector<T>> partitionA(InputIt first, InputIt last, unsigned size) {
        std::vector<std::vector<T>> result;
        std::vector<T>* batch{};
        for (unsigned index = 0, row = 0; first != last; ++first, ++index) {
            if ((index % size) == 0) {
                result.resize(++row);
                batch = &result.back();
                batch->reserve(size);
            }
            batch->push_back(*first);
        }
        return result;
    }


#endif
