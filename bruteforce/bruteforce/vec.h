/* FILE NAME   : VEC.H
 * PURPOSE     : vector library
 * PROGRAMMER  : AK5a.
 * LAST UPDATE : 18.07.2017
 */

#ifndef __VEC_H_
#define __VEC_H_

#include <ctime>
#include <iostream>
#include "MTHDEF.H"


/* Vector on plane class */
template<class Type>
class vec2
{
public:
  Type X, Z;                            /* Vector coordinates */ 

  vec2( Type x, Type z ) : X(x), Z(z)
  {
  }

  vec2( Type c = 0 ) : X(c), Z(c)
  {
  }

  DBL VecLen( void ) const
  {
    return sqrt(X * X + Z * Z);
  }

  DBL VecLen2(void) const
  {
    return (X * X + Z * Z);
  }

  vec2 operator!( void ) const
  {
    DBL l = this->VecLen();
    if (!l)
      l = 1;
    return vec2(X / l, Z / l);
  }

  bool operator==(const vec2& ot)
  {
    return X == ot.X && Z == ot.Z;
  }

  vec2 operator-( void ) const
  {
    return vec2(-X, -Z);
  }

  vec2 operator+( const vec2 & v ) const
  {
    return vec2(X + v.X, Z + v.Z);
  }

  vec2 operator-( const vec2 & v ) const
  {
    return vec2(X - v.X, Z - v.Z);
  }

  vec2 operator*( DBL n ) const
  {
    return vec2(X * n, Z * n);
  }

  vec2 operator/( DBL n ) const
  {
    return vec2(X / n, Z / n);
  }

  static DBL AngleVecPlane( const vec2 & v1, const vec2 & v2 )
  {
    vec2<Type> t1 = !v1, t2 = !v2;
    DBL c = acos((t1 & t2) / (t1.VecLen() * t2.VecLen()));

    return (t1.X * t2.Z - t2.X * t1.Z >= 0)? c : -c ;
  }

  Type operator&( const vec2 & v ) const
  {
    return X * v.X + Z * v.Z;
  }

  vec2 & operator=( const vec2 & v )
  {
    X = v.X;
    Z = v.Z;
    return *this;
  }

  vec2 & operator+=( const vec2 & v )
  {
    *this = *this + v;
    return *this;
  }

  vec2 & operator-=( const vec2 & v )
  {
    *this = *this - v;
    return *this;
  }

  vec2 & operator*=( DBL n )
  {
    *this = *this * n;
    return *this;
  }

  /* Double area actually */
  /* >0 if p3 lefter p1p2 */
  static Type SignedArea(vec2 p1, vec2 p2, vec2 p3)
  {
    return
        p1.X * p2.Z + p3.X * p1.Z + p2.X * p3.Z 
      - p3.X * p2.Z - p2.X * p1.Z - p1.X * p3.Z;
  }

  /* returns 1 if p1 > p2 */
  static int AngleCmp(vec2 p1, vec2 p2)
  {
    int a = int(SignedArea(vec2(0), p1, p2));
    return a > 0 ? 1 : (a == 0 ? 0 : -1);
  }

  /* returns 1 if p1 > p2 */
  static int AngleCmp(vec2 p0, vec2 p1, vec2 p2)
  {
    int a = int(SignedArea(p0, p1, p2));
    return a > 0 ? 1 : (a == 0 ? 0 : -1);
  }

  /* check q1 and q2 both left or both right of p1p2 */
  static bool CheckSameHalfplane(vec2 q1, vec2 q2, vec2 p1, vec2 p2)
  {
    int a1 = SignedArea(p1, p2, q1);
    int a2 = SignedArea(p1, p2, q2);
    return a1 * a2 > 0;
  }

  void Rotate(Type sine, Type cosine)
  {
    Type newX, newZ;
    newX = X * cosine - Z * sine;
    newZ = X * sine + Z * cosine;
    X = newX;
    Z = newZ;
  }
};

#endif // __VEC_H_

/* END OF 'VEC.H' FILE */
