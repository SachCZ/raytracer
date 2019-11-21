#ifndef RAYTRACER_JSON_FORMATTER_H
#define RAYTRACER_JSON_FORMATTER_H

#include <string>
#include <map>
#include <sstream>
#include "json/json.h"

namespace raytracer {
    namespace utility {

        typedef Json::Value JsonValue;

        class JsonFormatter {
        public:
            template<typename Sequence, typename ElementFormatter>
            static std::string getSequenceRepresentation(
                    const Sequence &sequence,
                    ElementFormatter elementFormatter)
            {
                std::stringstream stream;
                std::string separator;

                stream << "[";
                for (auto& element : sequence) {
                    stream << separator << elementFormatter(element);
                    separator = ", ";
                }
                stream << "]";

                Json::Value value;

                return stream.str();
            }

            static std::string getObjectRepresentation(const std::map<std::string, std::string>& map);
        };
    }
}

#endif //RAYTRACER_JSON_FORMATTER_H
