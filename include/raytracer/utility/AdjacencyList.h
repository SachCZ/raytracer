#ifndef RAYTRACER_ADJACENCY_LIST_H
#define RAYTRACER_ADJACENCY_LIST_H

#include <set>
#include <unordered_map>

namespace raytracer {
    namespace utility {

        class AdjacencyList {
        public:
            std::set<int> getAdjacent(int index) const;

            void addEdge(int i, int j);
        private:
            std::unordered_map<int, std::set<int>> list;

            void addHalfEdge(int i, int j);;
        };
    }
}



#endif //RAYTRACER_ADJACENCY_LIST_H
