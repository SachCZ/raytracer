#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker

if __name__ == "__main__":
    _, axis = plt.subplots()
    rayvis.plot_rays(axis, rayvis.read_rays("data/rays.json"), color="black", alpha=0.9, linewidth=0.1)

    nodes, elements = rayvis.read_vtk("data/mesh.vtk")
    rayvis.plot_vtk_mesh(axis, nodes, elements)

    formatter = ticker.ScalarFormatter(useMathText=True)
    formatter.set_scientific(True)
    formatter.set_powerlimits((-1,1))
    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$x$ [cm]")
    axis.set_ylabel("$y$ [cm]")

    plt.axis('equal')
    plt.savefig("images/bremsstrahlung_rays.svg", format='svg', dpi=1200)
    plt.clf()

    fig, axis = plt.subplots()
    values = rayvis.read_grid_function("data/absorbed_energy.txt")
    print(sum(values))

    rayvis.plot_grid_function(fig, axis, nodes, elements, values)

    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$x$ [cm]")
    axis.set_ylabel("$y$ [cm]")

    plt.axis('equal')
    plt.savefig("images/absorbed_bremsstrahlung_energy.png")