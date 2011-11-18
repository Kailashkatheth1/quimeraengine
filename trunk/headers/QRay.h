// [TERMS&CONDITIONS]

#ifndef __QRAY__
#define __QRAY__

#include "QBaseRay.h"
#include "QBaseOrb.h"
#include "EQIntersections.h"

using namespace Kinesis::QuimeraEngine::Tools::DataTypes;

namespace Kinesis
{
namespace QuimeraEngine
{
namespace Tools
{
namespace Math
{

/// <summary>
/// Represents a ray in the space, which consists of a origin point, or position, and a direction. The direction
/// symbolizes a line with only one end (which coincides with the origin) and that extends to the infinite.
/// </summary>
template<class VectorTypeOrigin, class VectorTypeDirection>
class QDllExport QRay : public QBaseRay<VectorTypeOrigin, VectorTypeDirection>
{
    // CONSTANTS
    // ---------------
public:

    /// <summary>
	/// A ray set at the coordinate origin and whose direction vector is null.
	/// </summary>
    static const QRay RayZero;


	// CONSTRUCTORS
	// ---------------
public:

	/// <summary>
	/// Default constructor.
	/// </summary>
	inline QRay()
    {
    }

    /// <summary>
    /// Constructor that receives a ray.
    /// </summary>
    /// <param name="baseRay">[IN] An existing ray.</param>
    inline QRay(const QBaseRay<VectorTypeOrigin, VectorTypeDirection>& baseRay) : QBaseRay<VectorTypeOrigin, VectorTypeDirection>(baseRay)
    {
    }

    /// <summary>
    /// Constructor that receives the ray's position and direction.
    /// </summary>
    /// <param name="vOrigin">[IN] Ray's position.</param>
    /// <param name="vDirection">[IN] Ray's direction.</param>
    inline QRay(const VectorTypeOrigin &vOrigin, const VectorTypeDirection &vDirection) : QBaseRay<VectorTypeOrigin, VectorTypeDirection>(vOrigin, vDirection)
    {
    }


	// METHODS
	// ---------------
public:

    /// <summary>
    /// Assign operator that receives another ray.
    /// </summary>
    /// <param name="ray">[IN] An existing ray.</param>
    /// <returns>
    /// A reference to this ray, after assignation.
    /// </returns>
    inline QRay& operator=(const QBaseRay<VectorTypeOrigin, VectorTypeDirection>& ray)
    {
        QBaseRay<VectorTypeOrigin, VectorTypeDirection>::operator=(ray);
        return *this;
    }

    /// <summary>
    /// Converts the ray's direction in its opposite.
    /// </summary>
    inline void Reverse()
    {
        this->Direction *= -QFloat::_1;
    }

    /// <summary>
    /// Obtains a reverted copy of the ray. Converts copy's direction in its opposite.
    /// </summary>
    /// <param name="vRevertedRay">[OUT] A reverted copy of the ray.</param>
    inline void Reverse(QBaseRay<VectorTypeOrigin, VectorTypeDirection>& vRevertedRay) const
    {
        vRevertedRay = *this;
        reinterpret_cast<QRay<VectorTypeOrigin, VectorTypeDirection>*>(&vRevertedRay)->Reverse();
    }

    /// <summary>
    /// Normalizes the ray, converting its direction in a unit vector.
    /// </summary>
    inline void Normalize()
    {
        this->Direction.Normalize();
    }

    /// <summary>
    /// Obtains a normalized copy of the ray. Normalizes the copy, converting its direction in a unit vector.
    /// </summary>
    /// <param name="vNormalizedRay">[OUT] A normalized copy of the ray.</param>
    inline void Normalize(QBaseRay<VectorTypeOrigin, VectorTypeDirection>& vNormalizedRay) const
    {
        vNormalizedRay = *this;
        reinterpret_cast<QRay<VectorTypeOrigin, VectorTypeDirection>*>(&vNormalizedRay)->Normalize();
    }

    /// <summary>
    /// Calculates any point that belongs to the ray, based on the distance to the ray's position.
    /// </summary>
    /// <param name="fDistance">[IN] Distance from the point which is to be found to the ray's position.</param>
    /// <param name="vRayPoint">[OUT] A point of the ray.</param>
    inline void GetPoint(const float_q &fDistance, VectorTypeOrigin& vRayPoint) const
    {
        // It's assumed that the ray's direction vector is normalized
        vRayPoint = this->Direction;
        vRayPoint *= fDistance;
        vRayPoint += this->Origin;
    }

    /// <summary>
    /// Checks if resident ray intersects with the provided orb. Ray must be normalized to ensure correct result.
    /// </summary>
    /// <param name="orb">[IN] The orb whose intersection with resident ray will be checked.</param>
    /// <returns>
    /// True if ray intersect orb, false otherwise.
    /// </returns>
    inline bool Intersection (const QBaseOrb<VectorTypeOrigin> &orb) const
    {
        // Converts all vectors to VectorTypeDirection, that always will be QVector2 or QVector3
        VectorTypeDirection vNewRayOrigin(this->Origin - orb.P);

        //  B/2 (instead of B) is calculed here to optimize comparison.
        const float_q &fB = vNewRayOrigin.DotProduct(this->Direction);

        // Since ray is normalized, A = 1.
        //const float_q &fA = this->Direction.DotProduct(this->Direction);

        const float_q &fC = vNewRayOrigin.DotProduct(vNewRayOrigin) - orb.Radius * orb.Radius;

        // Discriminant: B^2 - 4AC
        const float_q fD = fB*fB - fC;

        if (QFloat::IsNegative(fD)) // D = B^2 - 4AC < 0 => 0 intersections
            return false;
        else if ( QFloat::IsZero(fD) ) // D = B^2 - 4AC = 0 => 1 intersection
        {
            if ( QFloat::IsPositive(-fB) ) // T = -B/2A -> must be >= 0
                return true;
            else
                return false;
        }
        else
            return QFloat::IsLowerOrEquals(fB, sqrt(fD));

            // Explanation:

            // In this case, there are two posible intersections:
            //    T1 = -fB - sqrt(fD)   from (-B - sqrt(B*B - 4AC))/2A, where A = 1, B = 2*fB
            //    T2 = -fB + sqrt(fD)   from (-B + sqrt(B*B - 4AC))/2A, where A = 1, B = 2*fB
            //
            // One of T1 and T2 must be positive:
            // T1 >= 0  => fB <= -sqrt(fD)
            //    OR
            // T2 >= 0 => fB <= sqrt(fD)
            //
            // That is, is sufficient that fB <= sqrt(fD)
    }

    /// <summary>
	/// Computes the intersection point between resident ray and provided orb, if it exists.
	/// Ray must be normalized to ensure correct result.
	/// </summary>
	/// <param name="orb">[IN] The orb whose intersection with resident ray will be checked.</param>
	/// <param name="vPoint1">[OUT] First point where they intersect, if they do.</param>
	/// <param name="vPoint2">[OUT] Second point where they intersect, if they do.</param>
	/// <returns>
    /// An enumerated value which represents the number of intersections between the ray and the orb, and can take
    /// the following values: E_None, E_One and E_Two.
	/// </returns>
	/// <remarks>
	/// -If there's no intersection point, the output parameters won't be modified.
	/// -If there's one intersection point, the second output parameter won't be modified,
	/// and first output parameter is filled with the intersection point.
    /// -If there are two intersection points, both output parameters are filled with the intersection points, storing
    /// in the first output parameter the closest to the origin point of the ray.
	/// </remarks>
    EQIntersections IntersectionPoint(const QBaseOrb<VectorTypeOrigin> &orb, VectorTypeOrigin &vPoint1, VectorTypeOrigin &vPoint2) const
    {
        // We set all vectors to the same type that output parameters to allow operations
        const VectorTypeOrigin &vDirection(this->Direction);

        // We reduce ray and orb to origin, in order to simplify orb equation, and we calculate
        // the new ray origin point
        const VectorTypeOrigin &vNewRayOrigin(this->Point - orb.P);

        // We replace then in the orb equation to force it to verify the ray equation
        // vDirection^2*T^2 + 2*vNewA*vDirection*T + vNewA^2 - r^2 = 0
        // The intersections with ray follows this rules (extracted from quadratic ecuation):
        //       D = B^2 - 4AC = 0 => 1 intersection
        //       D = B^2 - 4AC < 0 => 0 intersections
        //       D = B^2 - 4AC > 0 => 2 intersections

        // Since ray is normalized, A = 1
        // const float_q &fA = this->Direction.DotProduct(this->Direction);

        const float_q &fB = QFloat::_2 * vNewRayOrigin.DotProduct(vDirection);
        const float_q &fC = vNewRayOrigin.DotProduct(vNewRayOrigin) - orb.Radius * orb.Radius;

        // Remember that a = 1 -> D = B^2 - 4AC = B^2 - 4C
        const float_q &fD = fB * fB - QFloat::_4 * fC;


        if (QFloat::IsNegative(fD)) // D = B^2 - 4AC < 0 => 0 intersections
            return EQIntersections::E_None;
        else if (QFloat::IsZero(fD)) // D = B^2 - 4AC = 0 => 1 intersection
        {
            // T = -B/2A -> Remember that a=1
            const float_q &fT = -fB*QFloat::_0_5;

            // The intersection is backwards the ray origin
            if (QFloat::IsNegative(fT))
                return EQIntersections::E_None;
            else
            {
                vPoint1 = vNewRayOrigin + fT * vDirection + orb.P;
                return EQIntersections::E_One;
            }
        }
        else // D = B^2 - 4AC > 0 => 2 intersections
        {
            const float_q &fSqrtD = sqrt(fD);

            // Closest intersection to ls.A. T1 = (-B - sqrt(D))/2A -> Remember that A = 1
            const float_q &fT1 = -(fB + fSqrtD)*QFloat::_0_5;

            // Farthest intersection to ls.A. T2 = (-B + sqrt(D))/2A -> Remember that A = 1
            const float_q &fT2 = (-fB + fSqrtD)*QFloat::_0_5;

            // Prevent rays with origin point inside orb, which must have only one intersection.
            const bool &bT1Fails = QFloat::IsNegative(fT1);
            const bool &bT2Fails = QFloat::IsNegative(fT2);

            if (bT1Fails && bT2Fails)
                return EQIntersections::E_None; // Shouldn't happen this :(
            else if (bT1Fails)  // One parameter is negative, there is only one intersection
            {
                vPoint1 = vNewRayOrigin + fT2 * vDirection + orb.P;
                return EQIntersections::E_One;
            }
            else if (bT2Fails) // One parameter is negative, there is only one intersection
            {
                vPoint1 = vNewRayOrigin + fT1 * vDirection + orb.P;
                return EQIntersections::E_One;
            }
            else // Most of the cases: two intersections.
            {
                vPoint1 = vNewRayOrigin + fT1 * vDirection + orb.P;
                vPoint2 = vNewRayOrigin + fT2 * vDirection + orb.P;
                return EQIntersections::E_Two;
            }
        }
    }

    /// <summary>
	/// Converts ray into a string representation with the following format:
	/// "R:P(VectorTypeOrigin::ToString),D(VectorTypeDirection::ToString)".
    /// </summary>
    /// <returns>
    /// The string representation with the format specified.
    /// </returns>
    string_q ToString() const
    {
        return QE_L("R:P(") + this->Origin.ToString() + QE_L("),D(") + this->Direction.ToString() + QE_L(")");
    }

};

//##################=======================================================##################
//##################			 ____________________________			   ##################
//##################			|							 |			   ##################
//##################		    |    CONSTANTS DEFINITIONS	 |			   ##################
//##################		   /|							 |\			   ##################
//##################			 \/\/\/\/\/\/\/\/\/\/\/\/\/\/			   ##################
//##################													   ##################
//##################=======================================================##################

template <class VectorTypeOrigin, class VectorTypeDirection>
const QRay<VectorTypeOrigin, VectorTypeDirection> QRay<VectorTypeOrigin, VectorTypeDirection>::RayZero(VectorTypeOrigin::ZeroVector, VectorTypeDirection::ZeroVector);

} //namespace Math
} //namespace Tools
} //namespace QuimeraEngine
} //namespace Kinesis

#endif // __QRAY__
// [TERMS&CONDITIONS]
