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
 * @section brems_sec Bremsstrahlung absorption
 * First make sure that you have raytracer properly setup and that you have a vtk mesh file which you wish to
 * be traced through. You can use the file bundled with this example in data folder. You can find the whole example
 * in raytracer/examples/sampleGradient.
 * @subsection brems_subsec1 Code explanation
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
 * @subsection brems_subsec2 Running the example and plotting
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
 * @image html traceBremsstrahlung/images/absorbed_bremsstrahlung_energy.png
 * @image html traceBremsstrahlung/images/bremsstrahlung_rays.png
 *
 *
 *
 * @page traceResonancePage Trace through mesh and absorb energy via resonance
 * @tableofcontents
 * @dontinclude traceResonance/traceResonance.cpp
 * A simple example demonstrating tracing and resonance model usage.
 *
 * @section resonance_sec Resonance absorption
 * This example is very similar to @ref traceBremsstrahlungPage. As it is the case, only the part of the code
 * that differ will be discussed here. Please make sure to read the @ref traceBremsstrahlungPage first.
 *
 * @subsection resonance_subsec1 Key differences
 * First a raytracer::Marker is initialized to mark the cells where a ray got reflected. Theses are the only cells
 * where the resonance absorption takes place:
 * @skipline Marker
 *
 * This raytracer::Marker is then used in raytracer::SnellsLaw as this is the model that does decide whether a
 * rey reflected or not:
 * @skip SnellsLaw snellsLaw
 * @until );
 *
 * The key difference is which model gets used in raytracer::AbsorptionController. In this case it is
 * raytracer::Resonance:
 *
 * @skip AbsorptionController
 * @until absorber.addModel
 *
 * @subsection Plotting
 * The code is run the same way as the @ref traceBremsstrahlungPage. Only the python script name is different.
 * In this case it is plot_resonance.py.
 * The results in this case are summarized at the next two images.
 *
 * @image html traceResonance/images/absorbed_resonance_energy.png
 * @image html traceResonance/images/resonance_rays.png
 *
 * @page sampleGradientPage Sampling of gradient models
 * @tableofcontents
 * @dontinclude sampleGradient/sampleGradient.cpp
 * On this page results of various gradient models sampling over rectangular grid is presented.
 *
 * @section gradient_sec Gradient sampling
 * The whole implementation of gradient sampling is not described here as it is needlessly technical. Rather the main
 * points are emphasized and the results are presented.
 *
 * @subsection gradient_subsec1 Sampling grid
 * The raytracer code is designed in such a way that a raytracer::Gradient can be evaluated only given a proper
 * raytracer::Intersection. Now to get these intersections sample raytracer::Mesh is created:
 * @skip mfemMesh
 * @until mesh
 *
 * Next an appropriate raytracer::Laser incoming from left covering the whole grid is initialized:
 * @skip Laser
 * @until );
 * @until );
 *
 * This raytracer::Laser is then traced through the raytracer::Mesh:
 * @skip laser.generateRays
 * @until laser.generateIntersections
 *
 * The resulting intersections omitting the ones at the border are shown in the following image:
 * @image html sampleGradient/images/mesh.png
 * At theses intersections the gradient will be sampled.
 *
 * @subsection gradient_subsec2 Calculating the gradient
 *
 * The gradient is calculated simply using:
 * @skip auto gradientValue
 * @until );
 * where the gradient represents the used raytracer::Gradient model. The model is initialized with analytical
 * density function @f$ f(x, y) = \sin(2\pi x) + \sin(2\pi y) @f$.
 *
 * @subsection gradient_subsec3 Sampling gradient generated by the least square method
 * The model is initialized in the following way:
 * @skipline LeastSquare
 *
 * It is then evaluated at the previously mentioned intersections. After that the x and y components are
 * plotted separately with the analytic counterparts. The resulting plots are at the two following images:
 *
 * @image html sampleGradient/images/ls_grad_x_compare.png
 * @image html sampleGradient/images/ls_grad_y_compare.png
 *
 * Also the difference between the sampled solutions and the analytical solutions is plotted. It is
 * at the following images:
 *
 * @image html sampleGradient/images/ls_error_x.png
 * @image html sampleGradient/images/ls_error_y.png
 *
 * @subsection gradient_subsec4 Sampling gradient generated by transforming to H1
 *
 * The model raytracer::H1Gradient is initialized in the following way:
 * @skipline H1Gradient
 *
 * Then the same way as the previous the gradient is sampled and plotted:
 *
 * @image html sampleGradient/images/h1_grad_x_compare.png
 * @image html sampleGradient/images/h1_grad_y_compare.png
 *
 * The difference is also plotted as it is in the previous case:
 *
 * @image html sampleGradient/images/h1_error_x.png
 * @image html sampleGradient/images/h1_error_y.png
 *
 * @page sampleModelsPage Functional dependence of physical models
 * @tableofcontents
 * @dontinclude sampleModels/sampleModels.cpp
 * On this page collisional frequency and index of rarefaction models are sampled at given conditions and
 * the results are presented.
 *
 * @section models_sec1 Collisional frequency sampling
 *
 * The raytracer::CollisionalFrequency model is sampled over temperatures from 0 to 500 eV. It is done with
 * electron density being @f$ 10^21 @f$, laser wavelength 800 nm and ionization equal to 22.
 * @skip return collisionalFrequency
 * @until .asDouble
 *
 * The result is shown at the following image using logarithmic scale on both axis:
 *
 * @image html sampleModels/images/frequency_n_e1e21_lamb800_Z22.png
 *
 * @section models_sec2 Index of refraction sampling
 * The raytracer::LaserRay::getRefractiveIndex model is sampled over densities from @f$ \frac{1}{2} @f$ critical
 * density to @f$ \frac{3}{2} @f$ critical density. It is done for temperature 150 eV and laser wavelength equal
 * 800 nm:
 *
 * @skipline laserRay.wavelength
 *
 * @skip auto frequency
 * @until return laserRay
 *
 * The result is shown at the following image:
 *
 * @image html sampleModels/images/refindex_T150_lamb800.png
 *
 * @mainpage Raytracer documentation
 * Raytracer is a code written to enable tracing of laser rays to be used in hydrodynamic simulations.
 *
 *
 * @section mainpage_sec1 Getting started
 * To get started it is recommended to read the page about tracing a laser through mesh: @ref traceBremsstrahlungPage.
 * Then you might want to refer to other pages describing the various physical models. That is:
 *
 * @ref traceResonancePage
 *
 * @ref sampleGradientPage
 *
 * @ref sampleModelsPage
 *
 */

#endif //RAYTRACER_RAYTRACER_H
