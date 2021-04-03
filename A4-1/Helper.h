#include <string>
#include <sstream>

template <typename T>
std::string print_array(T val){
    std::stringstream ss;
    ss<<"{";
    for(auto it=val.begin();it!=val.end();it++)
        ss<<*it<<" ";
    ss<<"}";
    return ss.str();
}

template <typename T>
std::string print_two_values(T val){
    std::stringstream ss;
    ss<<"{";
    for(auto it=val.begin();it!=val.end();it++)
        ss<<"{ "<<it->first<<","<<it->second<<"}"<<",";
    ss<<"}";
    return ss.str();
}

template <typename T>
std::string save_two_values(T val){
    std::stringstream ss;
    ss<<val.size()<<" ";
    for(auto it:val)
        ss<<(it).first<<" "<<(it).second<<" ";
    return ss.str();
}
