#include <sstream>

#include "JsonFormatter.h"

std::string raytracer::utility::JsonFormatter::getObjectRepresentation(const std::map<std::string, std::string> &map) {
    std::stringstream stream;
    std::string separator;

    stream << "{";
    for (auto& pair : map) {
        stream << separator << "\"" << pair.first << "\": " << pair.second;
        separator = ", ";
    }
    stream << "}";

    return stream.str();
}
