#include "laser.h"
#include "constants.h"
#include "propagation.h"
#include "refraction.h"
#include <stdexcept>

namespace raytracer {
    bool shouldReflect(const Vector &unitInterfaceNormal, const Vector &unitIncDir, double n1, double n2) {
        auto n = unitInterfaceNormal;
        auto l = unitIncDir;
        auto c = (-1) * n * l;
        if (c < 0) {
            c = -c;
        }
        const double r = n1 / n2;
        if (n2 < std::numeric_limits<double>::epsilon()) {
            return true;
        }
        auto root = 1 - r * r * (1 - c * c);
        return root < 0;
    }

    Vector calcRayBend(const Vector &unitInterfaceNormal, const Vector &unitIncDir, double n1, double n2) {
        auto n = unitInterfaceNormal;
        auto l = unitIncDir;
        auto c = (-1) * n * l;
        if (c < 0) {
            c = -c;
            n = (-1) * n;
        }
        const double r = n1 / n2;
        auto root = 1 - r * r * (1 - c * c);
        return r * l + (r * c - sqrt(root)) * n;
    }

    Vector calcRayReflect(const Vector &unitInterfaceNormal, const Vector &unitIncDir) {
        auto n = unitInterfaceNormal;
        auto l = unitIncDir;
        auto c = (-1) * n * l;
        if (c < 0) {
            c = -c;
            n = (-1) * n;
        }
        return l + 2 * c * n;
    }

    SnellsLawBend::SnellsLawBend(
            const Mesh *mesh,
            const MeshFunc *refractIndex,
            const Gradient* gradCalc,
            Vector *fallbackGrad
    ) :
            mesh(mesh),
            refractIndex(refractIndex),
            gradCalc(gradCalc),
            fallbackGrad(fallbackGrad) {}

    tl::optional<Vector> SnellsLawBend::operator()(
            const PointOnFace &pointOnFace,
            const Vector &direction
    ) {
        const auto previousElement = mesh->getFaceDirAdjElement(pointOnFace.face, -1 * direction);
        const auto nextElement = mesh->getFaceDirAdjElement(pointOnFace.face, direction);
        if (!nextElement) return {};
        auto gradient = gradCalc->get(pointOnFace);
        if (!gradient || gradient.value().getNorm() == 0) {
            if (fallbackGrad) {
                gradient = *fallbackGrad;
            } else {
                return {};
            }
        }

        const double n1 = previousElement ? refractIndex->getValue(*previousElement) : 1.0;
        const double n2 = refractIndex->getValue(*nextElement);

        auto unitGrad = 1 / gradient.value().getNorm() * gradient.value();
        auto unitDir = 1 / direction.getNorm() * direction;
        return calcRayBend(unitGrad, unitDir, n1, n2);
    }

    TotalReflect::TotalReflect(
            const Mesh *mesh,
            const MeshFunc *refractIndex,
            const Gradient* gradCalc,
            Marker *reflectMarker,
            Vector *fallbackGrad
    ) :
            mesh(mesh),
            refractIndex(refractIndex),
            gradCalc(gradCalc),
            reflectMarker(reflectMarker),
            fallbackGrad(fallbackGrad) {}

    tl::optional<Vector> TotalReflect::operator()(const PointOnFace &pointOnFace, const Vector &direction) {
        const auto previousElement = mesh->getFaceDirAdjElement(pointOnFace.face, -1 * direction);
        const auto nextElement = mesh->getFaceDirAdjElement(pointOnFace.face, direction);
        if (!nextElement) return {};
        auto gradient = gradCalc->get(pointOnFace);
        if (!gradient || gradient.value().getNorm() == 0) {
            if (fallbackGrad) {
                gradient = *fallbackGrad;
            } else {
                return {};
            }
        }

        const double n1 = previousElement ? refractIndex->getValue(*previousElement) : 1.0;
        const double n2 = refractIndex->getValue(*nextElement);

        auto unitGrad = 1 / gradient.value().getNorm() * gradient.value();
        auto unitDir = 1 / direction.getNorm() * direction;
        if (shouldReflect(unitGrad, unitDir, n1, n2)) {
            if (gradient.value() * direction < 0) {
                return direction;
            } else {
                if (reflectMarker) reflectMarker->mark(pointOnFace);
                return calcRayReflect(unitGrad, unitDir);
            }
        } else {
            return {};
        }
    }

    Density calcCritDens(const Length &wavelength) {
        auto m_e = constants::electron_mass;
        auto c = constants::speed_of_light;
        auto e = constants::electron_charge;

        auto constant = m_e * M_PI * std::pow(c, 2) / std::pow(e, 2);

        return {constant * std::pow(wavelength.asDouble, -2)};
    }

    void Marker::mark(const PointOnFace &pointOnFace) {
        marked.insert(pointOnFace.id);
    }

    void Marker::unmark(const PointOnFace &pointOnFace) {
        marked.erase(pointOnFace.id);
    }

    bool Marker::isMarked(const PointOnFace &pointOnFace) const {
        return marked.find(pointOnFace.id) != marked.end();
    }

    tl::optional<Vector>
    ContinueStraight::operator()(const PointOnFace &, const Vector &previousDirection) {
        return previousDirection;
    }

    double calcRefractIndex(double density, const Length &wavelength, double collFreq) {
        auto permittivity = impl::calcPermittivity(density, wavelength, collFreq);
        if (permittivity.real() < 0) return 0;
        auto root = std::sqrt(permittivity);
        if (std::isnan(root.real())) {
            throw std::logic_error("Nan index of refraction!");
        }
        return root.real();
    }

    std::complex<double> impl::calcPermittivity(double density, const Length &wavelength, double collFreq) {
        auto nu_ei = collFreq;
        auto n_e = density;
        auto m_e = constants::electron_mass;
        auto e = constants::electron_charge;
        auto omega = 2 * M_PI * constants::speed_of_light / wavelength.asDouble;
        auto omega_p2 = 4 * M_PI * e * e * n_e / m_e;

        auto term = omega_p2 / (omega * omega + nu_ei * nu_ei);
        return {1 - term, nu_ei / omega * term};
    }

    double calcInvBremssCoeff(double density, const Length &wavelength, double collFreq) {
        auto eps = impl::calcPermittivity(density, wavelength, collFreq);
        return 4 * M_PI / wavelength.asDouble * std::sqrt(eps).imag();
    }

    tl::optional<Vector> ReflectOnCritical::operator()(const PointOnFace &pointOnFace, const Vector &direction) {
        const auto nextElement = mesh->getFaceDirAdjElement(pointOnFace.face, direction);
        //const auto prevElement = mesh->getFaceDirAdjElement(pointOnFace.face, -1 * direction);
        if (!nextElement) return {};
        //const double n1 = prevElement ? refractIndex->getValue(*prevElement) : 1.0;
        //const double n2 = refractIndex->getValue(*nextElement);
        if (dens->getValue(*nextElement) > critDens) {
            auto gradient = gradCalc->get(pointOnFace);
            if (!gradient || gradient.value().getNorm() == 0) {
                if (fallbackGrad) {
                    gradient = *fallbackGrad;
                } else {
                    return {};
                }
            }
            auto unitGrad = 1 / gradient.value().getNorm() * gradient.value();
            auto unitDir = 1 / direction.getNorm() * direction;
            if (direction * gradient.value() < 0) {
                return direction;
            } else {
                if (marker) {
                    marker->mark(pointOnFace);
                }
                return calcRayReflect(unitGrad, unitDir);
            }
        } else {
            return {};
        }
    }
}