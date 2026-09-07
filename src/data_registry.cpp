#include "data_registry.h"
#include <fstream>
namespace data {
std::string Registry::load_text(const char* path) { std::ifstream f(path, std::ios::binary); return std::string((std::istreambuf_iterator<char>(f)), {}); }
int Registry::integer(const std::string& text, const char* key, int fallback) {
    const std::string needle = std::string("\"") + key + "\""; const auto pos=text.find(needle); if(pos==std::string::npos) return fallback;
    const auto colon=text.find(':',pos); if(colon==std::string::npos) return fallback; try{return std::stoi(text.substr(colon+1));}catch(...){return fallback;}
}
double Registry::number(const std::string& text, const char* key, double fallback) {
    const std::string needle = std::string("\"") + key + "\""; const auto pos=text.find(needle); if(pos==std::string::npos) return fallback;
    const auto colon=text.find(':',pos); if(colon==std::string::npos) return fallback; try{return std::stod(text.substr(colon+1));}catch(...){return fallback;}
}
std::string Registry::string_value(const std::string& text, const char* key, const std::string& fallback) {
    const std::string needle = std::string("\"") + key + "\""; const auto pos=text.find(needle); if(pos==std::string::npos) return fallback;
    const auto start=text.find('"', text.find(':', pos)+1); if(start==std::string::npos) return fallback; const auto end=text.find('"', start+1); if(end==std::string::npos) return fallback; return text.substr(start+1,end-start-1);
}
}
