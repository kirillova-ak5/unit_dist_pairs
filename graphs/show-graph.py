import ast
import math
import matplotlib.collections as mc
import matplotlib.pyplot as plt
import sys


def cycle(seq):
    it = iter(seq)
    try:
        first = next(it)
    except StopIteration:
        return

    yield first
    for item in it:
        yield item
    yield first


def show(edges):
    n = max(i for e in edges for i in e) + 1
    a = 2 * math.pi / n
    points = [(math.cos(a * i), math.sin(a * i)) for i in range(n)]
    segs = [(points[v1], points[v2]) for v1, v2 in edges]

    print(edges)
    plt.gca().add_collection(mc.LineCollection(segs))
    plt.plot(*zip(*cycle(points)), 'o')
    plt.gca().set_aspect('equal', adjustable='box')
    plt.show()


def main():
    fname = sys.argv[1]
    with open(fname) as f:
        graphs = tuple(map(ast.literal_eval, f))
    print(f'Enter graph number [0, {len(graphs)})')
    while True:
        i = int(input('> '))
        show(graphs[i])


main()
