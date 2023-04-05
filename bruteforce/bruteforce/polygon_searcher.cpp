#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include "mth.h"
#include "polygon_searcher.h"

// prevComb - inout
static bool GetNextCombination(int n, int k, int* prevComb)
{
  for (int i = 0; i < k; i++)
    if (prevComb[k - i - 1] < n - i - 1)
    {
      prevComb[k - i - 1]++;
      for (int j = k - i; j < k; j++)
        prevComb[j] = prevComb[j - 1] + 1;
      return true;
    }
  return false;
}

static vec SecondTop(std::stack<vec>& stk) {
  vec tempPoint = stk.top();
  stk.pop();
  vec res = stk.top();
  stk.push(tempPoint);
  return res;
}

bool polygon_searcher::IsConvex(int nVert, int* vertIndices)
{
  std::vector<vec> vertices(nVert);
  for (int i = 0; i < nVert; i++)
    vertices[i] = vec(vertIndices[i] % gridSize, vertIndices[i] / gridSize);
 
  std::sort(vertices.begin() + 1, vertices.end(), [&](const vec& v1, const vec& v2)
    {
      int cmpRes = vec::SignedArea(vertices[0], v1, v2);
      if (cmpRes)
        return cmpRes < 0;
      return ((vertices[0] - v2).VecLen2() > (vertices[0] - v1).VecLen2());
    });

  int newSize = 1;
  for (int i = 1; i < nVert; i++)
  {
    while (i < nVert - 1 && vec::AngleCmp(vertices[0], vertices[i], vertices[i + 1]) == 0)
      i++;
    vertices[newSize] = vertices[i];
    newSize++;
  }
  if (newSize < nVert)
    return false;

  std::stack<vec> stk;
  stk.push(vertices[0]); stk.push(vertices[1]); stk.push(vertices[2]);
  for (int i = 3; i < nVert; i++) {
    while (vec::AngleCmp(SecondTop(stk), stk.top(), vertices[i]) != -1)
      stk.pop();
    stk.push(vertices[i]);
  }

  return stk.size() == nVert;
}

void polygon_searcher::Process(void)
{
  int *curCombination = new int[polySize];
  for (int i = 0; i < polySize; i++)
    curCombination[i] = i;

  int gridFullSize = gridSize * gridSize;
  int numDiag = (polySize * (polySize - 1)) / 2;

  float* diagLenghts = new float[numDiag];

  do {
    if (!IsConvex(polySize, curCombination))
      continue;
    std::vector<vec> vertices(polySize);
    for (int i = 0; i < polySize; i++)
      vertices[i] = vec(curCombination[i] % gridSize, curCombination[i] / gridSize);

    for (int k = 0, i = 0; i < polySize - 1; i++)
      for (int j = i + 1; j < polySize; j++, k++)
      {
        diagLenghts[k] = (vertices[i] - vertices[j]).VecLen();
      }
    std::sort(diagLenghts, diagLenghts + numDiag);

    int curBegin = 0, maxBegin = 0;
    int curLen = 1, maxLen = 1;
    for (int i = 1; i < numDiag; i++)
    {
      for (int j = curBegin; j < i; j++)
      {
        if (diagLenghts[i] - diagLenghts[j] <= eps)
        {
          curLen++;
          break;
        }
        else
        {
          if (curLen > maxLen)
          {
            maxBegin = curBegin;
            maxLen = curLen;
          }
          curBegin++;
          curLen--;
        }
      }
    }
    if (maxLen > 5)
    std::cout << maxLen << '\n';
  } while (GetNextCombination(gridFullSize, polySize, curCombination));

  delete[] curCombination;
  delete[] diagLenghts;
}


void polygon_searcher::OutResult(void)
{
  vec arr[] = { {1, 1}, {1, 2}, {2, 1} };
  int idxs[] = { 11, 31, 33, 13 };
  //std::cout << IsConvex(4, idxs);
  Process();
}
