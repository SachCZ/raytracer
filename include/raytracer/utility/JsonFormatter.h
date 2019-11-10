#ifndef RAYTRACER_JSONFORMATTER_H
#define RAYTRACER_JSONFORMATTER_H

#include <string>
#include <sstream>
#include <map>

namespace raytracer {
    namespace utility {

        class JsonFormatter {
        public:
            template<typename Sequence, typename ElementFormatter>
            static std::string getSequenceRepresentation(const Sequence &sequence, ElementFormatter elementFormatter) {
                std::stringstream stream;
                std::string separator;

                stream << "[";
                for (auto& element : sequence) {
                    stream << separator << elementFormatter(element);
                    separator = ", ";
                }
                stream << "]";

                return stream.str();
            }

            static std::string getObjectRepresentation(const std::map<std::string, std::string>& map) {
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
        };

    }
}

#endif //RAYTRACER_JSONFORMATTER_H
