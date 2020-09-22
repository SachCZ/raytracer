#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker
import numpy as np

if __name__ == "__main__":
    formatter = ticker.ScalarFormatter(useMathText=True)
    formatter.set_scientific(True)
    formatter.set_powerlimits((-3,3))
    nodes, elements = rayvis.read_mesh("data/mesh_long.mesh")
    nodes = nodes * 1e4
    plt.close('all')

    fig, axis = plt.subplots()
    values = rayvis.read_grid_function("data/absorbed_energy.txt")
    print(sum(values))

    rayvis.plot_grid_function(fig, axis, nodes, elements, np.absolute(values), label="E [-]")
    rays = np.asarray(rayvis.read_rays("data/rays.json"))
    rayvis.plot_rays(axis, rays, scale=1e4, each_n=500, color="black", alpha=1, linewidth=0.1)

    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$x$ [μm]")
    axis.set_ylabel("$y$ [μm]")

    axis.set_xlim([0, 60])
    #axis.set_ylim([0.002, 0.008])

    #plt.axis('equal')
    #plt.savefig("images/absorbed_energy.png")
    plt.savefig("images/energy_angeled.png")


