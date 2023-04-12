import matplotlib.pyplot as plt
import os
from tqdm import tqdm

output_dir = "result_pics"

if __name__ == '__main__':
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)
    with open("res.txt", "r") as file:
        nres = int(file.readline())
        polySize = int(file.readline())
        gridSize = int(file.readline())
        for res_idx in tqdm(range(nres)):
            # poly = [[4, 0], [1, 3], [7, 3], [4, 6], [0, 7]]
            poly = [float(str) for str in file.readline().split()]
            # poly_x, poly_y = zip(*poly)
            poly_x = [poly[i * 2] for i in range(polySize)]
            poly_y = [poly[i * 2 + 1] for i in range(polySize)]
            plt.grid()
            ndiags = int(file.readline())
            diags = [[0, 2], [2, 3], [0, 3], [2, 4]]
            diags_raw = [float(str) for str in file.readline().split()]
            diags = [[diags_raw[i * 2], diags_raw[i * 2 + 1]] for i in range(ndiags)]
            for i in range(polySize - 1):
                for j in range(i, polySize):
                    diag_x = [poly_x[i], poly_x[j]]
                    diag_y = [poly_y[i], poly_y[j]]
                    if [i, j] in diags:
                        plt.plot(diag_x, diag_y, color = (0, 1, 0))
            plt.scatter(poly_x, poly_y, zorder=5)
            plt.axis('square')
            plt.savefig(output_dir + "/img" + str(res_idx) + ".png")
            plt.close()
        #plt.show()


