#ifndef RAYTRACER_MESH_FUNCTION_H
#define RAYTRACER_MESH_FUNCTION_H

#include <map>

#include "Quadrilateral.h"

namespace raytracer {
    namespace geometry {
        class MeshFunction {
        public:
            template <typename Function>
            void setAll(const std::vector<Quadrilateral>& quadrilaterals, Function function){
                for (const auto& quad : quadrilaterals){
                    this->values[quad.id] = function(quad);
                }
            }

            std::map<int, double> getValues(){
                return this->values;
            }

            double& operator[] (const Quadrilateral& quad){
                return this->values[quad.id];
            }

        private:
            std::map<int, double> values;
        };
    }
}



#endif //RAYTRACER_MESH_FUNCTION_H
