#ifndef __POLYGON_SEARCHER_H_
#define __POLYGON_SEARCHER_H_

#include <vector>

class polygon_searcher
{
  int gridSize = 8;
  int polySize = 5;
  float eps = 0.2;

  struct search_result
  {
    int *polyVertices;
    int nDiags;
    int *diags;
  };
  std::vector<search_result> results;
public:
  void OutResult(void);
  bool IsConvex(int nVert, int *vertices);
  bool IsConvexFast(int nVert, int* vertices);
  void Process(bool fastConvex);
  ~polygon_searcher(void);
};

#endif //!__POLYGON_SEARCHER_H_
