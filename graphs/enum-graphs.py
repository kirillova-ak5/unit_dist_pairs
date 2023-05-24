import itertools


def edges(n):
    return itertools.combinations(range(n), 2)


def make_graph(n, edges):
    g = tuple([] for _ in range(n))
    for v1, v2 in edges:
        g[v1].append(v2)
        g[v2].append(v1)
    return tuple(map(frozenset, g))


def is_connected(g):
    n = len(g)
    visited = [False] * n
    stack = [0]
    while stack:
        v = stack.pop()
        if visited[v]:
            continue
        visited[v] = True
        stack.extend(g[v])
    return all(visited)


def ccw(n, v1, v2, v3):
    v2 -= v1
    v3 -= v1
    while v2 < 0:
        v2 += n
    while v3 < 0:
        v3 += n
    return v2 < v3


def double_triangle(edges, g):
    for v1, v2 in edges:
        sides = tuple(ccw(len(g), v1, v2, v) for v in g[v1] & g[v2])
        if len(frozenset(sides)) < len(sides):
            return True
    return False


def hourglasses(g):
    n = len(g)
    for v1, e1 in enumerate(g):
        for v2, v4 in itertools.combinations(e1, 2):
            for v3 in g[v2] & g[v4]:
                if v3 != v1:
                    sides = (
                        ccw(n, v1, v2, v3),
                        ccw(n, v2, v3, v4),
                        ccw(n, v3, v4, v1),
                        ccw(n, v4, v1, v2)
                    )
                    if len(frozenset(sides)) > 1:
                        return True
    return False


def is_valid(n, edges):
    g = make_graph(n, edges)
    if min(map(len, g)) < 2:
        return False
    if not is_connected(g):
        return False
    if double_triangle(edges, g):
        return False
    if hourglasses(g):
        return False
    return True


def graphs(n, k):
    return (e for e in itertools.combinations(edges(n), k) if is_valid(n, e))


def rotations(n):
    r = tuple(range(n))
    for t in (r, r[::-1]):
        for i in range(n):
            yield t[i:] + t[:i]


def rotate(edges, p):
    return tuple(sorted(tuple(sorted(p[v] for v in e)) for e in edges))


def canonical_graphs(n, k):
    s = set()
    rs = tuple(rotations(n))
    for e in graphs(n, k):
        ce = min(rotate(e, p) for p in rs)
        if ce not in s:
            yield ce
            s.add(ce)


def main():
    n, k, c = map(int, input().split())
    for e in [graphs, canonical_graphs][c](n, k):
        print(e)


main()
