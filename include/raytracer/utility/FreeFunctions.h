#ifndef RAYTRACER_FREE_FUNCTIONS_H
#define RAYTRACER_FREE_FUNCTIONS_H

#include <vector>

namespace raytracer {
    namespace utility {
        template <typename T>
        std::vector<T> linspace(T a, T b, size_t N) {
            T h = (b - a) / static_cast<T>(N-1);
            std::vector<T> xs(N);
            typename std::vector<T>::iterator x;
            T val;
            for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
                *x = val;
            return xs;
        }

        template <typename T, typename Function>
        T integrateTrapz(Function function, T x, T dx){
            return dx * (function(x) + function(x + dx)) / 2.0;
        }
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
