#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker

if __name__ == "__main__":
    formatter = ticker.ScalarFormatter(useMathText=True)
    formatter.set_scientific(True)
    formatter.set_powerlimits((-3,3))
    nodes, elements = rayvis.read_mesh("data/mesh.mesh")
    nodes = nodes*1e4

    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2)

    values = rayvis.read_grid_function("data/density.txt")
    rayvis.plot_rays(ax1, rayvis.read_rays("data/rays.json"), scale=1e4, each_n=1000, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_grid_function(fig, ax1, nodes, elements, values, "$n_e$ [cm$^{-3}$]")
    ax1.set_title('Trajectories')
    ax1.yaxis.set_major_formatter(formatter)
    ax1.xaxis.set_major_formatter(formatter)
    ax1.set_xlabel("$x$ [μm]")
    ax1.set_ylabel("$y$ [μm]")

    ax2.set_title('Electron density')
    values = rayvis.read_grid_function("data/density.txt")
    rayvis.plot_rays(ax2, rayvis.read_rays("data/rays.json"), scale=1e4, each_n=500, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_grid_function(fig, ax2, nodes, elements, values, "$n_e$ [cm$^{-3}$]")
    ax2.yaxis.set_major_formatter(formatter)
    ax2.xaxis.set_major_formatter(formatter)
    ax2.set_xlabel("$x$ [μm]")
    ax2.set_ylabel("$y$ [μm]")
    ax2.set_ylim([43, 65])
    ax2.set_xlim([2, 8])

    ax3.set_title('Absorbed energy')
    values = rayvis.read_grid_function("data/absorbed_energy.txt")
    rayvis.plot_rays(ax3, rayvis.read_rays("data/rays.json"), scale=1e4, each_n=500, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_grid_function(fig, ax3, nodes, elements, values, "E [-]")
    ax3.yaxis.set_major_formatter(formatter)
    ax3.xaxis.set_major_formatter(formatter)
    ax3.set_xlabel("$x$ [μm]")
    ax3.set_ylabel("$y$ [μm]")
    ax3.set_ylim([43, 65])
    ax3.set_xlim([2, 8])

    ax4.set_title('Temperature')
    values = rayvis.read_grid_function("data/temperature.txt")
    rayvis.plot_rays(ax4, rayvis.read_rays("data/rays.json"), scale=1e4, each_n=500, color="black", alpha=0.9, linewidth=0.1)
    rayvis.plot_grid_function(fig, ax4, nodes, elements, values, "$T_e$ [eV]")
    ax4.yaxis.set_major_formatter(formatter)
    ax4.xaxis.set_major_formatter(formatter)
    ax4.set_xlabel("$x$ [μm]")
    ax4.set_ylabel("$y$ [μm]")
    ax4.set_ylim([43, 65])
    ax4.set_xlim([2, 8])

    plt.tight_layout()

    plt.savefig("absorption_angle.png")


