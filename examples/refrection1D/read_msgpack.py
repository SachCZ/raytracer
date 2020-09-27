import msgpack
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # Read msgpack file
    with open("data/rays.msgpack", "rb") as data_file:
        byte_data = data_file.read()

    data_loaded = msgpack.unpackb(byte_data)
    plt.plot(data_loaded[0]["x"], data_loaded[0]["y"])
    plt.show()