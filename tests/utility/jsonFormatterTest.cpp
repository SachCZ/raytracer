#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility/JsonFormatter.h>
#include <vector>
#include <map>

using namespace testing;
using JsonFormatter = raytracer::utility::JsonFormatter;

class json_formatter : public Test {
public:
    std::vector<int> sequence{1, 2, 3};
    std::map<std::string, std::string> object{{"key", "\"value\""}, {"prop", "3"}};
};

TEST_F(json_formatter, formats_sequences_correctly) {
    auto result = JsonFormatter::getSequenceRepresentation(sequence, [](int i){return i+1;});
    ASSERT_THAT(result, Eq("[2, 3, 4]"));
}

TEST_F(json_formatter, formats_objects_correctly) {
    auto result = JsonFormatter::getObjectRepresentation(object);
    ASSERT_THAT(result, Eq("{\"key\": \"value\", \"prop\": 3}"));
}
