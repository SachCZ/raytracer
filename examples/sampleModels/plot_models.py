from matplotlib import pyplot as plt
import json

if __name__ == "__main__":
    with open("data/models.json") as json_file:
        data = json.load(json_file)
        for title, function in data.items():
            x = function["x"]
            y = function["y"]
            plt.title(title)
            plt.xlabel(function["xlabel"])
            plt.ylabel(function["ylabel"])
            plt.grid()

            if function["logarithmic"]:
                plt.loglog(x, y)
            else:
                plt.plot(x, y)
            plt.savefig("images/" + function["imageName"])
            plt.clf()
