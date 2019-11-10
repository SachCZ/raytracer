#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility/AdjacencyList.h>

using namespace testing;

class initialized_adjacency_list : public Test {
public:
    using AdjacencyList = raytracer::utility::AdjacencyList;

    AdjacencyList adjacencyList;

    void SetUp( ) override {
        adjacencyList.addEdge(1, 2);
        adjacencyList.addEdge(1, 3);
        adjacencyList.addEdge(2, 3);
    }
};

TEST_F(initialized_adjacency_list, etries_have_correct_adjacent_values) {
    EXPECT_THAT(adjacencyList.getAdjacent(1), UnorderedElementsAreArray({3, 2}));
    EXPECT_THAT(adjacencyList.getAdjacent(2), UnorderedElementsAreArray({1, 3}));
    ASSERT_THAT(adjacencyList.getAdjacent(3), UnorderedElementsAreArray({1, 2}));
}