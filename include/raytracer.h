#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H

/**
 * @defgroup api Raytracer laser API
 * This is the user API, refer to to this module or its submodules if you are a user of Raytracer.
 * It is also highly recommended to use only abstractions from this API if you are not a developer of Raytracer.
 * @{
 *      @defgroup directionFinders Direction finders
 *      Functions and functors to be used with Laser::generateIntersections() as findDirection parameter.
 *      @defgroup intersectionFinders Intersection finders
 *      Functions and functors to be used with Laser::generateIntersections() as findIntersection parameter.
 *      @defgroup stopConditions Stop conditions
 *      Functions and functors to be used with Laser::generateIntersections() as stopCondition parameter.
 *      @defgroup gradients Gradients
 *      Gradient models to calculate gradient at given point in a MeshFunction.
 *      @defgroup frequency Collisional frequencies
 *      CollisionalFrequency models to calculate collisional frequency.
 *      @defgroup absorption Absorption models
 *      All possible AbsorptionModel implementations are documented in this module.
 * @}
 *
 * @page traceBremsstrahlungPage Trace through mesh and absorb energy via bremsstrahlung
 * @tableofcontents
 * @dontinclude traceBremsstrahlung/traceBremsstrahlung.cpp
 * A simple example demonstrating tracing and bremsstrahlung model usage.
 * @section sec Example breakdown
 * First make sure that you have raytracer properly setup and that you have a vtk mesh file which you wish to
 * be traced through. You can use the file bundled with this example in data folder. You can find the whole example
 * in raytracer/examples/sampleGradient.
 * @subsection subsec1 Code explanation
 * First lets define temperature, density and ionization functions that will be used to find a correct path
 * and absorbed energy for each ray:
 * @skip double
 * @until }
 * @until }
 * @until }
 * The executable code is defined in main, where the raytracer namespace is used:
 * @skip int main
 * @until using
 *
 * Make sure to load the vtk mesh and wrap it into raytracer::Mesh.
 * Besides that we will need a mfem::FiniteElementSpace:
 * @skip auto mfemMesh
 * @until mfem::FiniteElementSpace
 *
 * Now in real simulation it is expected that a mfem::GridFunction holding the absorbed energy is already defined.
 * Here we have to define it. In any case it must be wrapped by raytracer::MfemMeshFunction.
 * Note that raytracer::MfemMeshFunction holds a reference to the mfem::GridFunction and will crash if the
 * referenced GridFunction is deleted:
 * @skip mfem::GridFunction absorbedEnergyGridFunction
 * @until MfemMeshFunction absorbedEnergyMeshFunction
 *
 * In a similar fashion temperature, density and ionization grid functions are wrapped. In this example these have
 * the analytical forms defined at the beginning. This might seem as it could be simplified, but remember
 * that this is expected to be already defined:
 * @skip mfem::GridFunction densityGridFunction
 * @until MfemMeshFunction ionizationMeshFunction
 *
 * Now we will define the actual physical situation and models used.
 * @note The previous code is just a boilerplate, the actual usage of raytracer is best shown at following lines.
 *
 * A raytracer::Gradient calculation model is needed. Here we use the raytracer::LeastSquare gradient
 * which is initialized using mesh and density raytracer::MeshFunction.
 * @skipline LeastSquare
 *
 * A raytracer::CollisionalFrequency model is also necessary. Here a raytracer::SpitzerFrequency is used:
 * @skipline SpitzerFrequency
 *
 * To decide which direction should be take after encountering a mesh face a proper direction finder must be
 * specified. Such a functor is raytracer::SnellsLaw which as the name suggests uses Snell's law to determine
 * the direction. Initialize it using all the necessary models:
 * @skip SnellsLaw snellsLaw
 * @until );
 *
 * Now we will initialize an incoming raytracer::Laser. The laser begins on a line between points
 * (-0.51e-5, -0.3e-5) and (-0.51e-5, -0.5e-5). Its direction is in every point on the line (1, 0.7).
 * It has a gaussian profile of energy with FWHM 0.3e-5 and energy total of 1:
 * @skip Laser laser(
 * @until );
 * @until );
 *
 * All that is left for the tracing part of the simulation is to generate the laser rays for the given raytracer::Laser
 * and trace these through the raytracer::Mesh. We do this using generateRays and generateIntersections methods:
 * @skip laser.generateRays
 * @until laser.generateIntersections
 * raytracer::intersectStraight means that the cells are intersected straight, not that the whole ray is straight.
 * raytracer::DontStop just says the laser should be traced as long as it is possible.
 *
 * Now with laser that intersects the mesh its time to exchange energy between the laser and the hypothetical medium.
 * To do so initialize an raytracer::AbsorptionController.
 * @skipline AbsorptionController
 *
 * Specify the absorption models that are going to be used and register these to raytracer::AbsorptionController.
 * Here just one model is used, the raytracer::Bremsstrahlung:
 * @skip Bremsstrahlung
 * @until absorber.addModel
 *
 * Update the absorbedEnergy raytracer::MeshFunction accordingly:
 * @skipline absorber.absorb
 *
 * And at last write the output to files:
 * @skip laser.saveRaysToJson
 * @until absorbedEnergyGridFunction.Save
 *
 * @subsection subsec2 Running the example and plotting
 * To run the example compile it using cmake and your compiler of choice. Make sure to run it inside the
 * examples/sampleGradient folder as the paths to external files are hardcoded. Executing the script will
 * result in absorbed_energy.txt and rays.json being generated inside the data folder.
 *
 * To plot these make sure that you have python <a href="https://github.com/SachCZ/rayvis">rayvis</a> installed.
 * Then just run python3 plot_bremsstrahlung.py inside the same folder. This will generate two images
 * inside the images folder. One depicting the how the rays were traced through the mesh and one showing the
 * amount of absorbed energy due to bremsstrahlung.
 *
 *
 * @image html traceBremsstrahlung/images/absorbed_energy.png
 * @image html traceBremsstrahlung/images/rays.png
 *
 *
 */

#endif //RAYTRACER_RAYTRACER_H
