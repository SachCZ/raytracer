#ifndef RAYTRACER_POLYFILLS_H
#define RAYTRACER_POLYFILLS_H

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace raytracer {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

#endif //RAYTRACER_POLYFILLS_H
