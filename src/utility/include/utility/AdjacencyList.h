#ifndef RAYTRACER_ADJACENCYLIST_H
#define RAYTRACER_ADJACENCYLIST_H

#include <unordered_map>
#include <unordered_set>

namespace raytracer {
    namespace utility {

        class AdjacencyList {
        public:
            std::unordered_set<int> getAdjacent(int index) const {
                return this->list.at(index);
            }

            void addEdge(int i, int j) {
                this->addHalfEdge(i, j);
                this->addHalfEdge(j, i);
            }
        private:
            std::unordered_map<int, std::unordered_set<int>> list;

            void addHalfEdge(int i, int j) {
                if (this->list.find(i) != this->list.end()) { //list contains i
                    auto &set = this->list[i];
                    if (set.find(j) == set.end()) { //set does not contain j
                        set.insert(j);
                    }
                } else {
                    this->list[i] = {j};
                }
            };
        };
    }
}



#endif //RAYTRACER_ADJACENCYLIST_H
