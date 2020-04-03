#include <raytracer/physics/CollisionalFrequency.h>
#include <raytracer/utility/FreeFunctions.h>
#include <fstream>
#include <json/json.h>
#include <raytracer/physics/LaserRay.h>

struct FunctionMetadata {
    std::string title = "";
    std::string xLabel = "";
    std::string yLabel = "";
    std::string imageName = "";
    bool logarithmic = false;
};

template<typename Function, typename Points>
void dumpFunctionToJson(
        Function function,
        Points points,
        const std::string &filename,
        const FunctionMetadata &metadata
) {
    std::vector<double> result;
    result.reserve(points.size());
    for (const auto &point : points) {
        result.emplace_back(function(point));
    }

    std::ifstream inputFile(filename);
    Json::Value root;
    if (inputFile.is_open()){
        inputFile >> root;
        inputFile.close();
    }

    Json::Value object;
    for (int i = 0; i < result.size(); i++) {
        object["x"].append(points[i]);
        object["y"].append(result[i]);
    }
    object["xlabel"] = metadata.xLabel;
    object["ylabel"] = metadata.yLabel;
    object["logarithmic"] = metadata.logarithmic;
    object["imageName"] = metadata.imageName;
    root[metadata.title] = object;
    std::ofstream outputFile(filename);
    outputFile << root;
}

int main(int argc, char *argv[]) {
    using namespace raytracer;

    const std::string filename = "data/models.json";
    //Erase the file
    std::remove(filename.data());

    SpitzerFrequency collisionalFrequency;

    dumpFunctionToJson(
            [&collisionalFrequency](double temperature) {
                return collisionalFrequency.get(
                        Density{1e21},
                        Temperature{temperature},
                        Length{800e-7},
                        22
                ).asDouble;
            },
            linspace(0.0, 500.0, 1000),
            filename,
            FunctionMetadata{
                    "Spitzer frequency $n_e=10^{21}$, $\\lambda=800$ nm, $Z=22$",
                    "$T$ [eV]",
                    "$\\nu$ [s$^{-1}$]",
                    "frequency_n_e1e21_lamb800_Z22.png",
                    true
            }
    );

    LaserRay laserRay;
    laserRay.wavelength = Length{800e-7};
    auto criticalDensity = laserRay.getCriticalDensity().asDouble;

    dumpFunctionToJson(
        [&laserRay, &collisionalFrequency](double density) {
            auto frequency = collisionalFrequency.get(
                    Density{density},
                    Temperature{150},
                    laserRay.wavelength,
                    0
            );
            return laserRay.getRefractiveIndex(Density{density}, frequency);
        },
        linspace(criticalDensity - 0.5 * criticalDensity, criticalDensity + 0.5 * criticalDensity, 1000),
        filename,
        FunctionMetadata{
            "Refractive index at $T$ = 150 eV, $\\lambda = 800$ nm",
            "$n_e$ [cm^$-3$]",
            "$n$ [-]",
            "refindex_T150_lamb800.png"
        }
    );
}