#ifndef __POLYGON_SEARCHER_H_
#define __POLYGON_SEARCHER_H_

#include <vector>

class polygon_searcher
{
private:
  int gridSize = 8;
  int polySize = 8;
  float eps = 1.5;

  struct search_result
  {
    int *polyVertices;
    int nDiags;
    int *diags;
    std::vector<int*> masks;
  };
  std::vector<search_result> results;
  int * GeneratePrimaryMask(search_result& res);
  void GenerateMasksFromDiags(search_result &res);
public:
  void OutResult(void);
  bool IsConvex(int nVert, int *vertices);
  bool IsConvex1(int nVert, int* vertices);
  bool IsConvex2(int nVert, int* vertices);
  bool IsConvexFast(int nVert, int* vertices);
  void Process(bool fastConvex);
  ~polygon_searcher(void);
};

#endif //!__POLYGON_SEARCHER_H_
