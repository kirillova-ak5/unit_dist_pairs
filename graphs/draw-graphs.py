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


def draw(edges, fname):
    n = max(i for e in edges for i in e) + 1
    a = 2 * math.pi / n
    points = [(math.cos(a * i), math.sin(a * i)) for i in range(n)]
    segs = [(points[v1], points[v2]) for v1, v2 in edges]

    plt.clf()
    plt.gca().add_collection(mc.LineCollection(segs))
    plt.plot(*zip(*cycle(points)), 'o')
    plt.gca().set_aspect('equal', adjustable='box')
    plt.savefig(fname)


def main():
    for i, line in enumerate(sys.stdin):
        edges = ast.literal_eval(line)
        draw(edges, f'frame-{i}.png')


main()
