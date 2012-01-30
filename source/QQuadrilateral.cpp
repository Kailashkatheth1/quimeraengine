// [TERMS&CONDITIONS]

#include "QQuadrilateral.h"

using namespace Kinesis::QuimeraEngine::Tools::Math;

namespace Kinesis
{
namespace QuimeraEngine
{
namespace Tools
{
namespace Math
{


//##################=======================================================##################
//##################             ____________________________              ##################
//##################            |                            |             ##################
//##################            |            METHODS         |             ##################
//##################           /|                            |\            ##################
//##################             \/\/\/\/\/\/\/\/\/\/\/\/\/\/              ##################
//##################                                                       ##################
//##################=======================================================##################

bool QQuadrilateral::Contains (const QBaseVector2 &vPoint) const
{
    if (this->IsConvex()) // Its convex
    {
        return (PointsInSameSideOfLine(vPoint, this->C, this->A, this->B) &&
                PointsInSameSideOfLine(vPoint, this->D, this->B, this->C) &&
                PointsInSameSideOfLine(vPoint, this->A, this->C, this->D) &&
                PointsInSameSideOfLine(vPoint, this->B, this->D, this->A));
    }
    else if (this->IsCrossed()) // Its crossed
    {
        QLineSegment2D ls1(this->A, this->D), ls2(this->B, this->C);
        QVector2 vAux;

        if (ls1.IntersectionPoint(ls2, vAux) == EQIntersections::E_One) // Check if AD and BC edges cross themselves
        {
            // It checks both triangles (ABvAux and CDvAux)
            return (( PointsInSameSideOfLine(vPoint, vAux, this->A, this->B) &&
                      PointsInSameSideOfLine(vPoint, this->A, this->B, vAux) &&
                      PointsInSameSideOfLine(vPoint, this->B, vAux, this->A) ) ||
                    ( PointsInSameSideOfLine(vPoint, this->C, vAux, this->D) &&
                      PointsInSameSideOfLine(vPoint, vAux, this->D, this->C) &&
                      PointsInSameSideOfLine(vPoint, this->D, this->C, vAux) ));
        }
        else // AB and CD edges must cross themselves
        {
            // It checks both triangles (ADvAux and BCvAux)
            return (( PointsInSameSideOfLine(vPoint, this->A, vAux, this->D) &&
                  PointsInSameSideOfLine(vPoint, vAux, this->D, this->A) &&
                  PointsInSameSideOfLine(vPoint, this->D, this->A, vAux) ) ||
                ( PointsInSameSideOfLine(vPoint, this->B, this->C, vAux) &&
                  PointsInSameSideOfLine(vPoint, this->C, vAux, this->B) &&
                  PointsInSameSideOfLine(vPoint, vAux, this->B, this->C) ));
        }
    }
    else if (this->IsConcaveHere(this->A, this->B, this->D, this->C)) // Its concave in A vertex
    {
        // We check the two triangles around A vertex
        return (( PointsInSameSideOfLine(vPoint, this->C, this->A, this->B) &&
                  PointsInSameSideOfLine(vPoint, this->A, this->B, this->C) &&
                  PointsInSameSideOfLine(vPoint, this->B, this->C, this->A) ) ||
                ( PointsInSameSideOfLine(vPoint, this->C, this->A, this->D) &&
                  PointsInSameSideOfLine(vPoint, this->A, this->D, this->C) &&
                  PointsInSameSideOfLine(vPoint, this->D, this->C, this->A) ));
    }
    else if (this->IsConcaveHere(this->B, this->A, this->C, this->D)) // Its concave in B vertex
    {
        // We check the two triangles around B vertex
        return (( PointsInSameSideOfLine(vPoint, this->D, this->B, this->C) &&
                  PointsInSameSideOfLine(vPoint, this->B, this->C, this->D) &&
                  PointsInSameSideOfLine(vPoint, this->C, this->D, this->B) ) ||
                ( PointsInSameSideOfLine(vPoint, this->A, this->B, this->D) &&
                  PointsInSameSideOfLine(vPoint, this->B, this->D, this->A) &&
                  PointsInSameSideOfLine(vPoint, this->D, this->A, this->B) ));
    }
    else if (this->IsConcaveHere(this->C, this->B, this->D, this->A)) // Its concave in C vertex
    {
        // We check the two triangles around C vertex
        return (( PointsInSameSideOfLine(vPoint, this->A, this->C, this->D) &&
                  PointsInSameSideOfLine(vPoint, this->C, this->D, this->A) &&
                  PointsInSameSideOfLine(vPoint, this->D, this->A, this->C) ) ||
                ( PointsInSameSideOfLine(vPoint, this->B, this->C, this->A) &&
                  PointsInSameSideOfLine(vPoint, this->C, this->A, this->B) &&
                  PointsInSameSideOfLine(vPoint, this->A, this->B, this->C) ));
    }
    else // It must be concave in D vertex
    {
        // We check the two triangles around D vertex
        return (( PointsInSameSideOfLine(vPoint, this->B, this->D, this->A) &&
                  PointsInSameSideOfLine(vPoint, this->D, this->A, this->B) &&
                  PointsInSameSideOfLine(vPoint, this->A, this->B, this->D) ) ||
                ( PointsInSameSideOfLine(vPoint, this->C, this->D, this->B) &&
                  PointsInSameSideOfLine(vPoint, this->D, this->B, this->C) &&
                  PointsInSameSideOfLine(vPoint, this->B, this->C, this->D) ));
    }
}

bool QQuadrilateral::Intersection(const QBaseQuadrilateral &quad) const
{

    // Checks if any edge of resident quadrilateral intersects with q
    QLineSegment2D ls1(this->A, this->B);
    if ( ls1.Intersection(quad) )
        return true;

    ls1 = QLineSegment2D(this->B, this->C);
    if ( ls1.Intersection(quad) )
        return true;

    ls1 = QLineSegment2D(this->C, this->D);
    if ( ls1.Intersection(quad) )
        return true;

    ls1 = QLineSegment2D(this->D, this->A);
    if ( ls1.Intersection(quad) )
        return true;

    // q may be contained by resident quadrilateral, we check it.
    if ( this->Contains(quad.A) )
        return true;

    return false;
}

bool QQuadrilateral::IsCrossed() const
{
    return ( ( !PointsInSameSideOfLine(this->A, this->D, this->B, this->C) &&
               !PointsInSameSideOfLine(this->C, this->B, this->A, this->D) ) ||
             ( !PointsInSameSideOfLine(this->A, this->B, this->D, this->C) &&
               !PointsInSameSideOfLine(this->C, this->D, this->A, this->B) ) );
}

bool QQuadrilateral::IsConvex() const
{
    QLineSegment2D ls1(this->A, this->C);
    QLineSegment2D ls2(this->B, this->D);

    return ls1.Intersection(ls2);
}

bool QQuadrilateral::IsConcave() const
{
    return !this->IsConvex();
}

float_q QQuadrilateral::GetAngleA() const
{
    const float_q &fAngle = (this->D - this->A).DotProductAngle(this->B - this->A);

    if (this->IsConcaveHere(this->A, this->B, this->D, this->C))
    {
        #if QE_CONFIG_ANGLENOTATION_DEFAULT == QE_CONFIG_ANGLENOTATION_DEGREES
            return SQAngle::_360 - fAngle;
        #else
            return SQAngle::_2Pi - fAngle;
        #endif
    }
    else
        return fAngle;
}

float_q QQuadrilateral::GetAngleB() const
{
    const float_q &fAngle = (this->A - this->B).DotProductAngle(this->C - this->B);

    if (this->IsConcaveHere(this->B, this->A, this->C, this->D))
    {
        #if QE_CONFIG_ANGLENOTATION_DEFAULT == QE_CONFIG_ANGLENOTATION_DEGREES
            return SQAngle::_360 - fAngle;
        #else
            return SQAngle::_2Pi - fAngle;
        #endif
    }
    else
        return fAngle;
}

float_q QQuadrilateral::GetAngleC() const
{
    const float_q &fAngle = (this->B - this->C).DotProductAngle(this->D - this->C);

    if (this->IsConcaveHere(this->C, this->B, this->D, this->A))
    {
        #if QE_CONFIG_ANGLENOTATION_DEFAULT == QE_CONFIG_ANGLENOTATION_DEGREES
            return SQAngle::_360 - fAngle;
        #else
            return SQAngle::_2Pi - fAngle;
        #endif
    }
    else
        return fAngle;
}

float_q QQuadrilateral::GetAngleD() const
{
    const float_q &fAngle = (this->C - this->D).DotProductAngle(this->A - this->D);

    if (this->IsConcaveHere(this->D, this->A, this->C, this->B))
    {
        #if QE_CONFIG_ANGLENOTATION_DEFAULT == QE_CONFIG_ANGLENOTATION_DEGREES
            return SQAngle::_360 - fAngle;
        #else
            return SQAngle::_2Pi - fAngle;
        #endif
    }
    else
        return fAngle;
}

string_q QQuadrilateral::ToString()
{
    return QE_L("QL(a(") + this->A.ToString() + QE_L("),b(") +
                           this->B.ToString() + QE_L("),c(") +
                           this->C.ToString() + QE_L("),d(") +
                           this->D.ToString() + QE_L("))");
}

bool QQuadrilateral::PointsInSameSideOfLine(const QBaseVector2 &vPoint1, const QBaseVector2 &vPoint2,
                                            const QBaseVector2 &vLine1, const QBaseVector2 &vLine2) const
{

    const float_q &fOrientation1 = (vLine1.x - vPoint1.x)*(vLine2.y - vPoint1.y) - (vLine1.y - vPoint1.y)*(vLine2.x - vPoint1.x);
    const float_q &fOrientation2 = (vLine1.x - vPoint2.x)*(vLine2.y - vPoint2.y) - (vLine1.y - vPoint2.y)*(vLine2.x - vPoint2.x);

    if ( SQFloat::IsZero(fOrientation1) || SQFloat::IsZero(fOrientation2) )
        return true;
    else
        return ( SQFloat::IsNegative(fOrientation1) == SQFloat::IsNegative(fOrientation2) );
}


bool QQuadrilateral::IsConcaveHere(const QVector2 &vAngleVertex, const QVector2 &vAngleEndVertex1,
                                   const QVector2 &vAngleEndVertex2, const QVector2 &vOppositeVertex) const
{
    if (this->IsConvex() || this->IsCrossed()) // There isn't a concave angle.
        return false;
    else if ( !PointsInSameSideOfLine(vOppositeVertex, vAngleVertex, vAngleEndVertex1, vAngleEndVertex2) )
        return false; // The outer diagonal should not divide quadrilateral
    else
    {
        QLineSegment2D ls(vAngleEndVertex1, vAngleEndVertex2);

        // Angle vertex must be the closest to outer diagonal
        return ( ls.MinDistance(vAngleVertex) < ls.MinDistance(vOppositeVertex) );
    }
}

} //namespace Math
} //namespace Tools
} //namespace QuimeraEngine
} //namespace Kinesis
