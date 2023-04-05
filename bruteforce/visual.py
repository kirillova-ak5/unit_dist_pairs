import matplotlib.pyplot as plt


if __name__ == '__main__':
    poly = [[4, 0], [1, 3], [7, 3], [4, 6], [0, 7]]
    poly_x, poly_y = zip(*poly)
    plt.scatter(poly_x, poly_y)
    for i in range(4):
        for j in range(i, 5):
            diag_x = [poly_x[i], poly_x[j]]
            diag_y = [poly_y[i], poly_y[j]]
            plt.plot(diag_x, diag_y)
    plt.show()