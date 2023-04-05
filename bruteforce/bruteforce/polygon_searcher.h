#ifndef __POLYGON_SEARCHER_H_
#define __POLYGON_SEARCHER_H_

class polygon_searcher
{
  int gridSize = 8;
  int polySize = 5;
  float eps = 0.2;
public:
  void OutResult(void);
  bool IsConvex(int nVert, int *vertices);
  void Process(void);
};

#endif //!__POLYGON_SEARCHER_H_
