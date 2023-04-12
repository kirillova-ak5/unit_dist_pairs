#include <iostream>
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

// prevComb - inout
static bool GetNextCombinationPermuted(int n, int k, int* prevComb)
{
  if (std::next_permutation(prevComb, prevComb + k))
    return true;
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
bool polygon_searcher::IsConvexFast(int nVert, int* vertices)
{
  for (int i = 0; i < nVert; i++)
    if (vec::SignedArea(vertices[(i - 1 + nVert) % nVert], vertices[i], vertices[(i + 1 + nVert) % nVert]) < 0.f)
      return false;
  return true;
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

struct diag
{
  int i, j;
  float len;
};

void polygon_searcher::Process(bool fastConvex)
{
  int* curCombination = new int[polySize];
  for (int i = 0; i < polySize; i++)
    curCombination[i] = i;

  int gridFullSize = gridSize * gridSize;
  int numDiag = (polySize * (polySize - 1)) / 2;

  diag* diagLenghts = new diag[numDiag];

  do {
    if (fastConvex)
    {
      if (!IsConvexFast(polySize, curCombination))
        continue;
    }
    else
      if (!IsConvex(polySize, curCombination))
        continue;
    std::vector<vec> vertices(polySize);
    for (int i = 0; i < polySize; i++)
      vertices[i] = vec(curCombination[i] % gridSize, curCombination[i] / gridSize);

    for (int k = 0, i = 0; i < polySize - 1; i++)
      for (int j = i + 1; j < polySize; j++, k++)
      {
        diagLenghts[k].i = i;
        diagLenghts[k].j = j;
        diagLenghts[k].len = (vertices[i] - vertices[j]).VecLen();
      }
    std::sort(diagLenghts, diagLenghts + numDiag, [](const diag& d1, const diag& d2) 
      {
        return d1.len < d2.len;
      });

    int curBegin = 0, maxBegin = 0;
    int curLen = 1, maxLen = 1;
    for (int i = 1; i < numDiag; i++)
    {
      for (int j = curBegin; j < i; j++)
      {
        if (diagLenghts[i].len - diagLenghts[j].len <= eps)
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
    if (maxLen >= 2 * polySize - 4)
    {
      search_result res;
      res.polyVertices = new int[2 * polySize];
      res.nDiags = maxLen;
      res.diags = new int[2 * maxLen];
      for (int i = 0; i < polySize; i++)
      {
        res.polyVertices[i * 2] = vertices[i].X;
        res.polyVertices[i * 2 + 1] = vertices[i].Z;
      }
      for (int diagIdx = 0; diagIdx < maxLen; diagIdx++)
      {
        res.diags[diagIdx * 2] = diagLenghts[maxBegin + diagIdx].i;
        res.diags[diagIdx * 2 + 1] = diagLenghts[maxBegin + diagIdx].j;
      }
      results.push_back(res);
      if (maxLen > 6)
        std::cout << maxLen << '\n';
    }
  } while (fastConvex ? GetNextCombinationPermuted(gridFullSize, polySize, curCombination) : GetNextCombination(gridFullSize, polySize, curCombination));

  delete[] curCombination;
  delete[] diagLenghts;
}


void polygon_searcher::OutResult(void)
{
  vec arr[] = { {1, 1}, {1, 2}, {2, 1} };
  int idxs[] = { 11, 31, 33, 13 };
  int a[] = { 0, 1, 2 };
  //std::cout << IsConvex(4, idxs);
  Process(0);

  FILE* f = fopen("..\\res.txt", "w");
  if (!f)
    return;
  fprintf(f, "%i\n", results.size());
  fprintf(f, "%i\n", polySize);
  fprintf(f, "%i\n", gridSize);
  for (auto r : results)
  {
    for (int i = 0; i < polySize; i++)
    {
      fprintf(f, "%i %i ", r.polyVertices[i * 2], r.polyVertices[i * 2 + 1]);
    }
    fprintf(f, "\n%i\n", r.nDiags);
    for (int i = 0; i < r.nDiags; i++)
    {
      fprintf(f, "%i %i ", r.diags[i * 2], r.diags[i * 2 + 1]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

polygon_searcher::~polygon_searcher(void)
{
  for (auto r : results)
  {
    delete[] r.diags;
    delete[] r.polyVertices;
  }
}

