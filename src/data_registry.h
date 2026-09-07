#pragma once
#include <string>
namespace data {
class Registry {
public:
    static std::string load_text(const char* path);
    static int integer(const std::string& text, const char* key, int fallback);
    static double number(const std::string& text, const char* key, double fallback);
    static std::string string_value(const std::string& text, const char* key, const std::string& fallback);
};
}
