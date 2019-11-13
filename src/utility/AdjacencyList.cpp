#include "AdjacencyList.h"

std::set<int> raytracer::utility::AdjacencyList::getAdjacent(int index) const {
    return this->list.at(index);
}

void raytracer::utility::AdjacencyList::addEdge(int i, int j) {
    this->addHalfEdge(i, j);
    this->addHalfEdge(j, i);
}

void raytracer::utility::AdjacencyList::addHalfEdge(int i, int j) {
    if (this->list.find(i) != this->list.end()) { //list contains i
        auto &set = this->list[i];
        if (set.find(j) == set.end()) { //set does not contain j
            set.insert(j);
        }
    } else {
        this->list[i] = {j};
    }
}
