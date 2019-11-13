#ifndef RAYTRACER_JSON_FORMATTER_H
#define RAYTRACER_JSON_FORMATTER_H

#include <string>
#include <map>
#include <sstream>

namespace raytracer {
    namespace utility {

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

                return stream.str();
            }

            static std::string getObjectRepresentation(const std::map<std::string, std::string>& map);
        };
    }
}

#endif //RAYTRACER_JSON_FORMATTER_H
