#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker

if __name__ == "__main__":
    nodes, elements = rayvis.read_mesh("data/mesh.mesh")
    nodes = nodes*1e4

    fig, (ax1, ax2) = plt.subplots(1, 2)

    rayvis.plot_rays(ax1, rayvis.read_rays("data/rays1.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_rays(ax1, rayvis.read_rays("data/rays2.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_rays(ax1, rayvis.read_rays("data/rays3.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_rays(ax1, rayvis.read_rays("data/rays5.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    absorbed_energy = rayvis.read_grid_function("data/absorbed_energy.txt")
    rayvis.plot_grid_function(fig, ax1, nodes, elements, absorbed_energy, "E [-]", True)
    print(sum(absorbed_energy))
    ax1.set_xlabel("$x$ [μm]")
    ax1.set_ylabel("$y$ [μm]")
    ax1.set_xlim([4.1, 4.35])

    rayvis.plot_rays(ax2, rayvis.read_rays("data/rays1.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_rays(ax2, rayvis.read_rays("data/rays2.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_rays(ax2, rayvis.read_rays("data/rays3.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_rays(ax2, rayvis.read_rays("data/rays5.json"), each_n=100, color="black", alpha=0.9, linewidth=0.1)
    ax2.set_xlabel("$x$ [μm]")
    ax2.set_ylabel("$y$ [μm]")

    plt.tight_layout()

    plt.savefig("images/resonant_absorption.png")


