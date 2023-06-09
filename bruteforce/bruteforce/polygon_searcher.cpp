#include <iostream>
#include <stack>
#include <algorithm>
#include <chrono>
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


static vec SecondTop(std::vector<vec>& stk)
{
  vec tempPoint = stk.back();
  stk.pop_back();
  vec res = stk.back();
  stk.push_back(tempPoint);
  return res;
}

bool polygon_searcher::IsConvexFast(int nVert, int* vertIndices)
{
  std::vector<vec> vertices(nVert);
  for (int i = 0; i < nVert; i++)
    vertices[i] = vec(vertIndices[i] % gridSize, vertIndices[i] / gridSize);

  for (int i = -1; i < nVert; i++)
    if (vec::SignedArea(vertices[(i - 1 + nVert) % nVert], vertices[(i + nVert) % nVert], vertices[(i + 1 + nVert) % nVert]) <= 0.f)
      return false;
  return true;
}

bool polygon_searcher::IsConvex(int nVert, int* vertIndices)
{
  static std::vector<vec> vertices(nVert);
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

  static std::vector<vec> stk;
  stk.clear();
  stk.reserve(polySize);
  stk.push_back(vertices[0]); stk.push_back(vertices[1]); stk.push_back(vertices[2]);
  for (int i = 3; i < nVert; i++) {
    while (vec::AngleCmp(SecondTop(stk), stk.back(), vertices[i]) != -1)
      stk.pop_back();
    stk.push_back(vertices[i]);
  }

  if (stk.size() != nVert)
    return false;
  for (int i = 0; i < nVert; i++)
  {
    vec v = stk.back();
    stk.pop_back();
    vertIndices[i] = v.X + v.Z * gridSize;
  }
  return true;
}

bool polygon_searcher::IsConvex1(int nVert, int* vertIndices)
{
  static std::vector<vec> vertices(nVert);
  for (int i = 0; i < nVert; i++)
    vertices[i] = vec(vertIndices[i] % gridSize, vertIndices[i] / gridSize);

  if (vec::SignedArea(vertices[0], vertices[1], vertices[2]) == 0)
    return false;
  if (vec::SignedArea(vertices[0], vertices[1], vertices[2]) < 0)
    std::swap(vertices[1], vertices[2]);

  for (int curChecked = 3; curChecked < nVert; curChecked++)
  {
    int countRight = 0;
    int lastRight = -1;
    for (int i = 0; i < curChecked; i++)
    {
      float area = vec::SignedArea(vertices[i], vertices[(i + 1) % curChecked], vertices[curChecked]);
      if (area == 0)
        return false;
      if (area < 0)
      {
        countRight++;
        lastRight = (i + 1) % curChecked;
        if (countRight > 1)
          return false;
      }
    }
    if (countRight != 1)
      return false;
    vec v = vertices[curChecked];
    vertices.erase(vertices.begin() + curChecked);
    vertices.insert(vertices.begin() + lastRight, v);
  }
  for (int i = 0; i < nVert; i++)
  {
    vertIndices[i] = vertices[i].X + vertices[i].Z * gridSize;
  }
  return true;
}


struct diag
{
  int i, j;
  float len;
};

int* polygon_searcher::GeneratePrimaryMask(search_result& res)
{
  int* mask = new int[polySize];
  memset(mask, 0, sizeof(int) * polySize);
  for (int i = 0; i < res.nDiags; i++)
  {
    mask[res.diags[i * 2]] += 1 << res.diags[i * 2 + 1];
    mask[res.diags[i * 2 + 1]] += 1 << res.diags[i * 2];
  }
  return mask;
}

void polygon_searcher::GenerateMasksFromDiags(search_result& res)
{
  res.masks.resize(polySize * 2);
  for (int numeration = 0; numeration < polySize; numeration++)
  {
    int* maskFwd = new int[polySize];
    int* maskInv = new int[polySize];
    memset(maskFwd, 0, sizeof(int) * polySize);
    memset(maskInv, 0, sizeof(int) * polySize);
    for (int i = 0; i < res.nDiags; i++)
    {
      maskFwd[(res.diags[i * 2] + numeration) % polySize] += 1 << ((res.diags[i * 2 + 1] + numeration) % polySize);
      maskFwd[(res.diags[i * 2 + 1] + numeration) % polySize] += 1 << ((res.diags[i * 2] + numeration) % polySize);
      maskInv[(polySize - res.diags[i * 2] - 1 + numeration) % polySize] += 1 << ((polySize - res.diags[i * 2 + 1] - 1+ numeration) % polySize);
      maskInv[(polySize - res.diags[i * 2 + 1] - 1 + numeration) % polySize] += 1 << ((polySize - res.diags[i * 2] - 1 + numeration) % polySize);
    }
    res.masks[numeration] = maskFwd;
    res.masks[polySize + numeration] = maskInv;
  }
}


void polygon_searcher::Process(bool fastConvex)
{
  int* curCombination = new int[polySize];
  int* curCombinationCopy = new int[polySize];
  for (int i = 0; i < polySize; i++)
    curCombination[i] = i;

  int gridFullSize = gridSize * gridSize;
  int numDiag = (polySize * (polySize - 1)) / 2;

  diag* diagLenghts = new diag[numDiag];

  do {
    memcpy(curCombinationCopy, curCombination, sizeof(int) * polySize);
    if (fastConvex)
    {
      if (!IsConvexFast(polySize, curCombination))
        continue;
    }
    else
      if (!IsConvex1(polySize, curCombinationCopy))
        continue;
    std::vector<vec> vertices(polySize);
    for (int i = 0; i < polySize; i++)
      vertices[i] = vec(curCombinationCopy[i] % gridSize, curCombinationCopy[i] / gridSize);

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
    // filter interesting ones
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
      // filter false positive

      // sandglass
      int checkVert1[] = { 0, 1, 2, 3 };
      bool falsePos = false;
      do
      {
        bool d[4] = { 0 };
        for (int pair = 0; pair < maxLen; pair++)
        {
          d[0] = d[0] || (res.diags[pair * 2] == checkVert1[0] && res.diags[pair * 2 + 1] == checkVert1[1]) || (res.diags[pair * 2 + 1] == checkVert1[0] && res.diags[pair * 2] == checkVert1[1]);
          d[1] = d[1] || (res.diags[pair * 2] == checkVert1[1] && res.diags[pair * 2 + 1] == checkVert1[2]) || (res.diags[pair * 2 + 1] == checkVert1[1] && res.diags[pair * 2] == checkVert1[2]);
          d[2] = d[2] || (res.diags[pair * 2] == checkVert1[2] && res.diags[pair * 2 + 1] == checkVert1[3]) || (res.diags[pair * 2 + 1] == checkVert1[2] && res.diags[pair * 2] == checkVert1[3]);
          d[3] = d[3] || (res.diags[pair * 2] == checkVert1[3] && res.diags[pair * 2 + 1] == checkVert1[0]) || (res.diags[pair * 2 + 1] == checkVert1[3] && res.diags[pair * 2] == checkVert1[0]);
        }
        if (d[0] && d[1] && d[2] && d[3])
        {
          int inv = 0;
          for (int k = 0; k < 4; k++)
            if (checkVert1[k] > checkVert1[(k + 1) % 4])
            {
              inv++;
            }
          if (inv % 2 == 0)
          {
            falsePos = true;
            break;
          }
        }
      } while (GetNextCombinationPermuted(polySize, 4, checkVert1));
      if (falsePos)
        continue;

      // 3 trangles
      int checkVert[] = { 0, 1, 2, 3, 4 };
      do
      {
        bool d[7] = { 0 };
        for (int pair = 0; pair < maxLen; pair++)
        {
          d[0] = d[0] || (res.diags[pair * 2] == checkVert[0] && res.diags[pair * 2 + 1] == checkVert[1]) || (res.diags[pair * 2 + 1] == checkVert[0] && res.diags[pair * 2] == checkVert[1]);
          d[1] = d[1] || (res.diags[pair * 2] == checkVert[0] && res.diags[pair * 2 + 1] == checkVert[2]) || (res.diags[pair * 2 + 1] == checkVert[0] && res.diags[pair * 2] == checkVert[2]);
          d[2] = d[2] || (res.diags[pair * 2] == checkVert[0] && res.diags[pair * 2 + 1] == checkVert[3]) || (res.diags[pair * 2 + 1] == checkVert[0] && res.diags[pair * 2] == checkVert[3]);
          d[3] = d[3] || (res.diags[pair * 2] == checkVert[0] && res.diags[pair * 2 + 1] == checkVert[4]) || (res.diags[pair * 2 + 1] == checkVert[0] && res.diags[pair * 2] == checkVert[4]);
          d[4] = d[4] || (res.diags[pair * 2] == checkVert[1] && res.diags[pair * 2 + 1] == checkVert[2]) || (res.diags[pair * 2 + 1] == checkVert[1] && res.diags[pair * 2] == checkVert[2]);
          d[5] = d[5] || (res.diags[pair * 2] == checkVert[2] && res.diags[pair * 2 + 1] == checkVert[3]) || (res.diags[pair * 2 + 1] == checkVert[2] && res.diags[pair * 2] == checkVert[3]);
          d[6] = d[6] || (res.diags[pair * 2] == checkVert[3] && res.diags[pair * 2 + 1] == checkVert[4]) || (res.diags[pair * 2 + 1] == checkVert[3] && res.diags[pair * 2] == checkVert[4]);
        }
        if (d[0] && d[1] && d[2] && d[3] && d[4] && d[5] && d[6])
        {
            falsePos = true;
            break;
        }
      } while (GetNextCombinationPermuted(polySize, 5, checkVert));
      if (falsePos)
        continue;


      // filter same graphs
      int* curMask = GeneratePrimaryMask(res);
      bool accept = true;
      if (results.size() == 5)
        std::cout << "";
      for (auto r : results)
      {
        if (!accept)
          break;
        for (auto mask : r.masks)
          if (!memcmp(curMask, mask, sizeof(int) * polySize))
          {
            accept = false;
            break;
          }
      }
      delete[] curMask;
      if (accept)
      {
        GenerateMasksFromDiags(res);
        results.push_back(res);
      }
      if (maxLen > 6)
        ;// std::cout << maxLen << '\n';
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
  //while (GetNextCombinationPermuted(4, 3, a))
   // std::cout << a[0]<<a[1]<<a[2]<<'\n';
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
  Process(0);
  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
 // Process(1);
  std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> time_span0 = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t2 - t1);
  std::chrono::duration<double, std::milli> time_span1 = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t3 - t2);

  std::cout << time_span0.count() << ' ' << time_span1.count();

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
    for (auto m : r.masks)
      delete[] m;
  }
}

