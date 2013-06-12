//-------------------------------------------------------------------------------//
//                         QUIMERA ENGINE : LICENSE                              //
//-------------------------------------------------------------------------------//
// This file is part of Quimera Engine.                                          //
// Quimera Engine is free software: you can redistribute it and/or modify        //
// it under the terms of the Lesser GNU General Public License as published by   //
// the Free Software Foundation, either version 3 of the License, or             //
// (at your option) any later version.                                           //
//                                                                               //
// Quimera Engine is distributed in the hope that it will be useful,             //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// Lesser GNU General Public License for more details.                           //
//                                                                               //
// You should have received a copy of the Lesser GNU General Public License      //
// along with Quimera Engine. If not, see <http://www.gnu.org/licenses/>.        //
//                                                                               //
// This license doesn't force you to put any kind of banner or logo telling      //
// that you are using Quimera Engine in your project but we would appreciate     //
// if you do so or, at least, if you let us know about that.                     //
//                                                                               //
// Enjoy!                                                                        //
//                                                                               //
// Kinesis Team                                                                  //
//-------------------------------------------------------------------------------//

#ifndef __QDUALQUATERNION__
#define __QDUALQUATERNION__

#include "QBaseDualQuaternion.h"

using namespace Kinesis::QuimeraEngine::Tools::DataTypes;

namespace Kinesis
{
namespace QuimeraEngine
{
namespace Tools
{
namespace Math
{

// Forward declarations
class QBaseVector3;
class QBaseVector4;


/// <summary>
/// Class which implements the basic functionality of a dual quaternion.<br>
/// A dual quaternion is a complex number with two components, a non-dual component and a dual one, e.g. \f$ r + d\varepsilon \f$.<br>
/// Both components (r and d) of a dual quaternion are (regular) quaternions.<br>
/// The dual unit, called epsilon (\f$ \varepsilon \f$), verifies \f$ \varepsilon^2=0 \f$. Then, a full quaternion looks like:
/// <br>
/// \f$ r_w + r_xi + r_yj + r_zk + (d_w + d_xi + d_yj + d_zk)\varepsilon\f$
/// <br>
/// We'll normally employ it to represent and execute rigid transformations, that is, transformations which involves
/// one rotation and one translation, without deforming the transformed solid (which acts as a rigid solid).
/// </summary>
/// <remarks>
/// This class has been included for experimental purposes only. Use it at your own risk.
/// </remarks>
class QDllExport QDualQuaternion : public QBaseDualQuaternion
{
    // FRIENDS
    // ---------------
public:

    /// <summary>
    /// Multiply by scalar operator. All dual quaternion components are multiplied by the scalar.
    /// </summary>
    /// <param name="fScalar">[IN] The scalar factor.</param>
    /// <param name="dualQuat">[IN] The dual quaternion factor.</param>
    /// <returns>
    /// The resultant dual quaternion.
    /// </returns>
    friend QDualQuaternion operator*(const float_q &fScalar, const QDualQuaternion &dualQuat);


	// CONSTRUCTORS
	// ---------------
public:

	/// <summary>
	/// Default constructor.
	/// </summary>
    inline QDualQuaternion()
    {
    }

    /// <summary>
    /// Copy constructor.
    /// </summary>
    /// <param name="dualQuat">[IN] The dual quaternion from which we want to create a copy in the resident dual quaternion.</param>
    inline QDualQuaternion(const QDualQuaternion &dualQuat) : QBaseDualQuaternion(dualQuat)
    {
    }

    /// <summary>
    /// Base type constructor.
    /// </summary>
    /// <param name="dualQuat">[IN] The dual quaternion from which we want to create a copy in the resident dual quaternion.</param>
    inline QDualQuaternion(const QBaseDualQuaternion &dualQuat) : QBaseDualQuaternion(dualQuat)
    {
    }

    /// <summary>
    /// Constructor from two regular quaternions.
    /// </summary>
    /// <param name="qReal">[IN] Regular quaternion to define non-dual part.</param>
    /// <param name="qDual">[IN] Regular quaternion to define dual part.</param>
    inline QDualQuaternion(const QBaseQuaternion &qReal, const QBaseQuaternion &qDual) : QBaseDualQuaternion(qReal, qDual)
    {
    }

    /// <summary>
    /// Constructor from a regular quaternion which represents a rotation and a vector which represents a translation.<br>
    /// The rotation regular quaternion is built from an angle (\f$\theta\f$) and a unit vector \f$\vec{n}(n_x, n_y, n_z)\f$
    /// in the direction of the rotation axis as follows:
    /// <br>
    /// \f$ q(x, y, z, w) = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2}))\f$,
    /// <br>
    /// and the dual quaternion will be:
    /// <br>
    /// \f$\hat{q}_r = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2})) (0, 0, 0, 0)\f$.
    /// <br>
    /// In the other side, the translation \f$(d_x, d_y, d_z)\f$ is directly converted to a dual quaternion as follows:
    /// <br>
    /// \f$\hat{q}_d = (0, 0, 0, 1) (\frac{d_x}{2}, \frac{d_y}{2}, \frac{d_z}{2}, 0)\f$.
    /// <br>
    /// Our transformation dual quaternion is given by the product of the translation dual quaternion by
    /// the rotation dual quaternion, taking into account that the transfomation is done as follows:<br>
    /// FIRST ROTATION THEN TRANSLATION.
    /// </summary>
    /// <param name="qRotation">[IN] The quaternion that keeps the rotation.</param>
    /// <param name="vTranslation">[IN] The vector which represents the translation.</param>
    QDualQuaternion(const QBaseQuaternion &qRotation, const QBaseVector3 &vTranslation)
    {
        QDualQuaternionImp(qRotation, vTranslation);
    }

    /// <summary>
    /// Constructor from a regular quaternion which represents a rotation and a vector which represents a translation.<br>
    /// The rotation regular quaternion is built from an angle (\f$\theta\f$) and a unit vector \f$\vec{n}(n_x, n_y, n_z)\f$
    /// in the direction of the rotation axis as follows:
    /// <br>
    /// \f$ q(x, y, z, w) = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2}))\f$,
    /// <br>
    /// and the dual quaternion will be:
    /// <br>
    /// \f$\hat{q}_r = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2})) (0, 0, 0, 0)\f$.
    /// <br>
    /// In the other side, the translation \f$(d_x, d_y, d_z)\f$ is directly converted to a dual quaternion as follows:
    /// <br>
    /// \f$\hat{q}_d = (0, 0, 0, 1) (\frac{d_x}{2}, \frac{d_y}{2}, \frac{d_z}{2}, 0)\f$.
    /// <br>
    /// Our transformation dual quaternion is given by the product of the translation dual quaternion by
    /// the rotation dual quaternion, taking into account that the transfomation is done as follows:<br>
    /// FIRST ROTATION THEN TRANSLATION.
    /// </summary>
    /// <remarks>
    /// Remember that the quaternion must be normalized to represent the rotation properly.
    /// </remarks>
    /// <param name="qRotation">[IN] The quaternion that keeps the rotation.</param>
    /// <param name="vTranslation">[IN] The vector which represents the translation.</param>
    QDualQuaternion(const QBaseQuaternion &qRotation, const QBaseVector4 &vTranslation)
    {
        QDualQuaternionImp(qRotation, vTranslation);
    }

    /// <summary>
    /// Constructor from a regular quaternion which represents a rotation and a vector which represents a translation.<br>
    /// The rotation regular quaternion is built from an angle (\f$\theta\f$) and a unit vector \f$\vec{n}(n_x, n_y, n_z)\f$
    /// in the direction of the rotation axis as follows:
    /// <br>
    /// \f$ q(x, y, z, w) = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2}))\f$,
    /// <br>
    /// and the dual quaternion will be:
    /// <br>
    /// \f$\hat{q}_r = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2})) (0, 0, 0, 0)\f$.
    /// <br>
    /// In the other side, the translation \f$(d_x, d_y, d_z)\f$ is directly converted to a dual quaternion as follows:
    /// <br>
    /// \f$\hat{q}_d = (0, 0, 0, 1) (\frac{d_x}{2}, \frac{d_y}{2}, \frac{d_z}{2}, 0)\f$.
    /// <br>
    /// Our transformation dual quaternion is given by the product of the rotation dual quaternion by
    /// the translation dual quaternion, taking into account that the transfomation is done as follows:<br>
    /// FIRST TRANSLATION THEN ROTATION.
    /// </summary>
    /// <remarks>
    /// Remember that the quaternion must be normalized to represent the rotation properly.
    /// </remarks>
    /// <param name="vTranslation">[IN] The vector which represents the translation.</param>
    /// <param name="qRotation">[IN] The quaternion that keeps the rotation.</param>
    QDualQuaternion(const QBaseVector3 &vTranslation, const QBaseQuaternion &qRotation)
    {
        QDualQuaternionImp(vTranslation, qRotation);
    }

    /// <summary>
    /// Constructor from a regular quaternion which represents a rotation and a vector which represents a translation.<br>
    /// The rotation regular quaternion is built from an angle (\f$\theta\f$) and a unit vector \f$\vec{n}(n_x, n_y, n_z)\f$
    /// in the direction of the rotation axis as follows:
    /// <br>
    /// \f$ q(x, y, z, w) = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2}))\f$,
    /// <br>
    /// and the dual quaternion will be:
    /// <br>
    /// \f$\hat{q}_r = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2})) (0, 0, 0, 0)\f$.
    /// <br>
    /// In the other side, the translation \f$(d_x, d_y, d_z)\f$ is directly converted to a dual quaternion as follows:
    /// <br>
    /// \f$\hat{q}_d = (0, 0, 0, 1) (\frac{d_x}{2}, \frac{d_y}{2}, \frac{d_z}{2}, 0)\f$.
    /// <br>
    /// Our transformation dual quaternion is given by the product of the rotation dual quaternion by
    /// the translation dual quaternion, taking into account that the transfomation is done as follows:<br>
    /// FIRST TRANSLATION THEN ROTATION.
    /// </summary>
    /// <remarks>
    /// Remember that the quaternion must be normalized to represent the rotation properly.
    /// </remarks>
    /// <param name="vTranslation">[IN] The vector which represents the translation.</param>
    /// <param name="qRotation">[IN] The quaternion that keeps the rotation.</param>
    QDualQuaternion(const QBaseVector4 &vTranslation, const QBaseQuaternion &qRotation)
    {
        QDualQuaternionImp(vTranslation, qRotation);
    }

    /// <summary>
    /// Constructor from two four components arrays of floating point values, one for each quaternion component.
    /// </summary>
    /// <param name="arValuesReal">[IN] Pointer to array of floating point values. It must have at least four elements.</param>
    /// <param name="arValuesDual">[IN] Pointer to array of floating point values. It must have at least four elements.</param>
    inline QDualQuaternion(const float_q *arValuesReal, const float_q *arValuesDual)
    {
        QE_ASSERT(arValuesReal != null_q && arValuesDual != null_q)

        this->r = QQuaternion(arValuesReal[0], arValuesReal[1], arValuesReal[2], arValuesReal[3]);
        this->d = QQuaternion(arValuesDual[0], arValuesDual[1], arValuesDual[2], arValuesDual[3]);
    }

protected:

    // <summary>
    // Constructor from a regular quaternion which represents a rotation and a vector which represents a translation.
    // The rotation regular quaternion is built from an angle (\f$\theta\f$) and a unit vector \f$\vec{n}(n_x, n_y, n_z)\f$
    // in the direction of the rotation axis as follows:
    // \f$ q(x, y, z, w) = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2}))\f$,
    // and the dual quaternion will be:
    // \f$\hat{q}_r = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2})) (0, 0, 0, 0)\f$.
    // In the other side, the translation \f$(d_x, d_y, d_z)\f$ is directly converted to a dual quaternion as follows:
    // \f$\hat{q}_d = (0, 0, 0, 1) (\frac{d_x}{2}, \frac{d_y}{2}, \frac{d_z}{2}, 0)\f$.
    // Our transformation dual quaternion is given by the product of the translation dual quaternion by
    // the rotation dual quaternion, taking into account that the transfomation is done as follows:
    // FIRST ROTATION THEN TRANSLATION.
    // </summary>
    // <param name="qRotation">[IN] The quaternion that keeps the rotation.</param>
    // <param name="vTranslation">[IN] The vector which represents the translation.</param>
    template <class VectorType>
    void QDualQuaternionImp(const QBaseQuaternion &qRotation, const VectorType &vTranslation)
    {
        QDualQuaternion rotation(qRotation, QBaseQuaternion(SQFloat::_0, SQFloat::_0, SQFloat::_0, SQFloat::_0));
        QDualQuaternion translation(QBaseQuaternion(SQFloat::_0, SQFloat::_0, SQFloat::_0, SQFloat::_1),
                                    QBaseQuaternion(vTranslation.x * SQFloat::_0_5, vTranslation.y * SQFloat::_0_5, vTranslation.z * SQFloat::_0_5, SQFloat::_0));

        *this = translation * rotation;
    }

    // <summary>
    // Constructor from a regular quaternion which represents a rotation and a vector which represents a translation.
    // The rotation regular quaternion is built from an angle (\f$\theta\f$) and a unit vector \f$\vec{n}(n_x, n_y, n_z)\f$
    // in the direction of the rotation axis as follows:
    // \f$ q(x, y, z, w) = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2}))\f$,
    // and the dual quaternion will be:
    // \f$\hat{q}_r = (n_xsin(\frac{\theta}{2}), n_ysin(\frac{\theta}{2}), n_zsin(\frac{\theta}{2}), cos(\frac{\theta}{2})) (0, 0, 0, 0)\f$.
    // In the other side, the translation \f$(d_x, d_y, d_z)\f$ is directly converted to a dual quaternion as follows:
    // \f$\hat{q}_d = (0, 0, 0, 1) (\frac{d_x}{2}, \frac{d_y}{2}, \frac{d_z}{2}, 0)\f$.
    // Our transformation dual quaternion is given by the product of the rotation dual quaternion by
    // the translation dual quaternion, taking into account that the transfomation is done as follows:
    // FIRST TRANSLATION THEN ROTATION.
    // </summary>
    // <param name="vTranslation">[IN] The vector which represents the translation.</param>
    // <param name="qRotation">[IN] The quaternion that keeps the rotation.</param>
    template <class VectorType>
    void QDualQuaternionImp(const VectorType &vTranslation, const QBaseQuaternion &qRotation)
    {
        QDualQuaternion rotation(qRotation, QBaseQuaternion(SQFloat::_0, SQFloat::_0, SQFloat::_0, SQFloat::_0));
        QDualQuaternion translation(QBaseQuaternion(SQFloat::_0, SQFloat::_0, SQFloat::_0, SQFloat::_1),
                                    QBaseQuaternion(vTranslation.x * SQFloat::_0_5, vTranslation.y * SQFloat::_0_5, vTranslation.z * SQFloat::_0_5, SQFloat::_0));

        *this = rotation * translation;
    }


    // PROPERTIES
    // ---------------
public:

    /// <summary>
    /// Gets a dual quaternion that represents the identity dual quaternion. It's (0,0,0,1) (0,0,0,0).
    /// </summary>
    /// <returns>
    /// The identity dual quaternion.
    /// </returns>
    inline static const QDualQuaternion& GetIdentity()
    {
        static const QDualQuaternion IDENTITY(QBaseQuaternion(SQFloat::_0, SQFloat::_0, SQFloat::_0, SQFloat::_1),
                                              QBaseQuaternion(SQFloat::_0, SQFloat::_0, SQFloat::_0, SQFloat::_0));
        return IDENTITY;
    }


	// METHODS
	// ---------------
public:

    /// <summary>
    /// Dual quaternions addition: it's performed component by component.
    /// </summary>
    /// <param name="dualQuat">[IN] Adding dual quaternion.</param>
    /// <returns>
    /// A dual quaternion that is the result of the addition.
    /// </returns>
    QDualQuaternion operator+(const QBaseDualQuaternion &dualQuat) const;

    /// <summary>
    /// Dual quaternions subtraction: it's performed component by component.
    /// </summary>
    /// <param name="dualQuat">[IN] Subtracting dual quaternion.</param>
    /// <returns>
    /// A dual quaternion that is the result of the subtraction.
    /// </returns>
    QDualQuaternion operator-(const QBaseDualQuaternion &dualQuat) const;

    /// <summary>
    /// Dual quaternions product: it's performed as follows:
    /// <br>
    /// \f$ q\cdot q' = r\cdot r' + (r\cdot d' + d\cdot r')\varepsilon\f$
    /// <br>
    /// </summary>
    /// <remarks>
    /// Product between dual quaternions is not commutative.
    /// </remarks>
    /// <param name="dualQuat">[IN] Multiplying dual quaternion.</param>
    /// <returns>
    /// A dual quaternion that is the result of the product.
    /// </returns>
    QDualQuaternion operator*(const QBaseDualQuaternion &dualQuat) const;

    /// <summary>
    /// Multiply by scalar operator. Both quaternion components are multiplied by the scalar.
    /// </summary>
    /// <param name="fScalar">[IN] The scalar to multiply by.</param>
    /// <returns>
    /// The resultant dual quaternion.
    /// </returns>
    QDualQuaternion operator*(const float_q &fScalar) const;

    /// <summary>
    /// Product by a vector (multiplying to the dual quaternion by the right).<br>
    /// The vector is transformed into a dual quaternion,
    /// then is multiplied by the resident dual quaternion by the left and transformed back to a vector.<br>
    /// If vector is \f$ \vec{v}=(x, y, z)\f$, it's transformed into \f$ \hat{Q}=(0, 0, 0, 1)(x, y, z, 0)\f$.
    /// </summary>
    /// <param name="vVector">[IN] Multiplying vector.</param>
    /// <returns>
    /// A vector that is the result of the product.
    /// </returns>
    QDualQuaternion operator*(const QBaseVector3 &vVector) const;

    /// <summary>
    /// Product by a vector (multiplying to the dual quaternion by the right).<br>
    /// The vector is transformed into a dual quaternion,
    /// then is multiplied by the resident dual quaternion by the left and transformed back to a vector.<br>
    /// If vector is \f$ \vec{v}=(x, y, z)\f$, it's transformed into \f$ \hat{Q}=(0, 0, 0, 1)(x, y, z, 0)\f$.
    /// </summary>
    /// <param name="vVector">[IN] Multiplying vector.</param>
    /// <returns>
    /// A vector that is the result of the product.
    /// </returns>
    QDualQuaternion operator*(const QBaseVector4 &vVector) const;

    /// <summary>
    /// Divide by scalar operator. Both quaternion components are divided by the scalar.
    /// </summary>
    /// <param name="fScalar">[IN] The scalar to divide by.</param>
    /// <returns>
    /// The resultant dual quaternion.
    /// </returns>
    QDualQuaternion operator/(const float_q &fScalar) const;

    /// <summary>
    /// Add and assign operator. Each input quaternion component is added to the corresponding quaternion
    /// of the resident dual quaternion.
    /// </summary>
    /// <param name="dualQuat">[IN] The dual quaternion that is Added.</param>
    /// <returns>
    /// The modified dual quaternion.
    /// </returns>
    inline QDualQuaternion& operator+=(const QBaseDualQuaternion &dualQuat)
    {
        this->r += dualQuat.r;
        this->d += dualQuat.d;

        return *this;
    }

    /// <summary>
    /// Subtract and assign operator. Each input quaternion component is subtracted to the corresponding quaternion
    /// of the resident dual quaternion.
    /// </summary>
    /// <param name="dualQuat">[IN] The dual quaternion that is subtracted.</param>
    /// <returns>
    /// The modified dual quaternion.
    /// </returns>
    inline QDualQuaternion& operator-=(const QBaseDualQuaternion &dualQuat)
    {
        this->r -= dualQuat.r;
        this->d -= dualQuat.d;

        return *this;
    }

    /// <summary>
    /// Multiply and assign operator. The resident dual quaternion is multiplied by the input one.<br>
    /// This is calculated as follows:
    /// <br>
    /// \f$ q\cdot q' = r\cdot r' + (r\cdot d' + d\cdot r')\varepsilon\f$
    /// <br>
    /// Note that quaternion multiplication is not conmutative.
    /// </summary>
    /// <param name="dualQuat">[IN] The dual quaternion to multiply by.</param>
    /// <returns>
    /// The modified dual quaternion.
    /// </returns>
    inline QDualQuaternion& operator*=(const QBaseDualQuaternion &dualQuat)
    {
        QDualQuaternion aux(*this);

        this->r = aux.r * dualQuat.r;
        this->d = aux.r * dualQuat.d + aux.d * dualQuat.r;

        return *this;
    }

    /// <summary>
    /// Multiply by a floating point value and assign operator. Each dual quaternion component is
    /// multiplied by the input floating point value.
    /// </summary>
    /// <param name="fScalar">[IN] The floating point value to multiply by.</param>
    /// <returns>
    /// The modified dual quaternion.
    /// </returns>
    inline QDualQuaternion& operator*=(const float_q fScalar)
    {
        this->r *= fScalar;
        this->d *= fScalar;

        return *this;
    }

    /// <summary>
    /// Divide and assign operator. Each dual quaternion component is divided by the input floating point value.
    /// </summary>
    /// <param name="fScalar">[IN] The floating point value to divide by.</param>
    /// <returns>
    /// The modified dual quaternion.
    /// </returns>
    inline QDualQuaternion& operator/=(const float_q &fScalar)
    {
        // Checkout to avoid division by zero.
        QE_ASSERT(fScalar != SQFloat::_0)

        const float_q &DIVISOR = SQFloat::_1/fScalar;

        this->r *= DIVISOR;
        this->d *= DIVISOR;

        return *this;
    }

    /// <summary>
    /// Assign operator. Assigns the provided dual quaternion to the resident dual quaternion.
    /// </summary>
    /// <param name="dualQuat">[IN] The dual quaternion to be assigned.</param>
    /// <returns>
    /// A reference to the modified dual quaternion.
    /// </returns>
    inline QDualQuaternion& operator=(const QBaseDualQuaternion &dualQuat)
    {
        QBaseDualQuaternion::operator=(dualQuat);
        return *this;
    }

    /// <summary>
    /// Resets to zero all components of both quaternions of the dual quaternion.
    /// </summary>
    inline void ResetToZero()
    {
        this->r.ResetToZero();
        this->d.ResetToZero();
    }

    /// <summary>
    /// Resets to identity the dual quaternion. This implies that de non-dual component is reset to identity
    /// and de dual component is reset to zero.
    /// </summary>
    inline void ResetToIdentity()
    {
        this->r.ResetToIdentity();
        this->d.ResetToZero();
    }

    /// <summary>
    /// Calculates the SINGLE dual quaternion's conjugate. It's calculated this way:
    /// <br>
    /// \f$ Q^* = r^* + d^*\varepsilon\f$ .
    /// </summary>
    /// <returns>
    /// The single dual quaternion conjugate.
    /// </returns>
    inline QDualQuaternion Conjugate() const
    {
        return QDualQuaternion(r.Conjugate(), d.Conjugate());
    }

    /// <summary>
    /// Calculates the DOUBLE dual quaternion's conjugate. It's calculated this way:
    /// <br>
    /// \f$ Q^* = r^* - d^*\varepsilon\f$.
    /// </summary>
    /// <returns>
    /// The double dual quaternion conjugate.
    /// </returns>
    inline QDualQuaternion DoubleConjugate() const
    {
        return QDualQuaternion(r.Conjugate(), d.Conjugate() * -SQFloat::_1);
    }

    /// <summary>
    /// Calculates the length of the non-dual part of the resident dual quaternion. It is used to normalize
    /// an interpolated transformation. See Lerp method for further information.
    /// </summary>
    /// <returns>
    /// A floating point value that is the length of the non-dual part of the dual quaternion.
    /// </returns>
    inline float_q GetNonDualLength() const
    {
        return this->r.GetLength();
    }

    /// <summary>
    /// Applies a transformation to the resident dual quaternion. The transformation is performed as follows:
    /// <br>
    ///     \f$ P' = T \cdot P \cdot T^*\f$, where
    /// <br>
    ///     \f$ P'\f$: Transformed dual quaternion.<br>
    ///     \f$ T \f$: Transformation applied.<br>
    ///     \f$ P \f$: Dual quaternion which is transformed.<br>
    ///     \f$ T^*\f$: Double conjugate of the transformation.
    /// </summary>
    /// <param name="transformation">[IN] Transformation to be applied.</param>
    /// <returns>
    /// The transformed dual quaternion.
    /// </returns>
    QDualQuaternion Transform(const QBaseDualQuaternion &transformation) const;

    /// <summary>
    /// Applies a transformation composed of a rotation and a translation, performing the rotation first and then the traslation.
    /// </summary>
    /// <param name="qRotation">[IN] Regular quaternion which defines the rotation.</param>
    /// <param name="vTranslation">[IN] Vector which defines the translation.</param>
    /// <returns>
    /// The transformed dual quaternion.
    /// </returns>
    QDualQuaternion TransformRotationFirst(const QBaseQuaternion &qRotation, const QBaseVector3 &vTranslation) const
    {
        return this->TransformRotationFirstImp(qRotation, vTranslation);
    }

    /// <summary>
    /// Applies a transformation composed of a rotation and a translation, performing the rotation first and then the traslation.
    /// </summary>
    /// <param name="qRotation">[IN] Regular quaternion which defines the rotation.</param>
    /// <param name="vTranslation">[IN] Vector which defines the translation.</param>
    /// <returns>
    /// The transformed dual quaternion.
    /// </returns>
    QDualQuaternion TransformRotationFirst(const QBaseQuaternion &qRotation, const QBaseVector4 &vTranslation) const
    {
        return this->TransformRotationFirstImp(qRotation, vTranslation);
    }

    /// <summary>
    /// Applies a transformation composed of a rotation and a translation, performing the traslation first and then the rotation.
    /// </summary>
    /// <param name="vTranslation">[IN] Vector which defines the translation.</param>
    /// <param name="qRotation">[IN] Regular quaternion which defines the rotation.</param>
    /// <returns>
    /// The transformed dual quaternion.
    /// </returns>
    QDualQuaternion TransformTranslationFirst(const QBaseVector3 &vTranslation, const QBaseQuaternion &qRotation) const
    {
        return this->TransformTranslationFirstImp(vTranslation, qRotation);
    }

    /// <summary>
    /// Applies a transformation composed of a rotation and a translation, performing the traslation first and then the rotation.
    /// </summary>
    /// <param name="vTranslation">[IN] Vector which defines the translation.</param>
    /// <param name="qRotation">[IN] Regular quaternion which defines the rotation.</param>
    /// <returns>
    /// The transformed dual quaternion.
    /// </returns>
    QDualQuaternion TransformTranslationFirst(const QBaseVector4 &vTranslation, const QBaseQuaternion &qRotation) const
    {
        return this->TransformTranslationFirstImp(vTranslation, qRotation);
    }

    /// <summary>
    /// Returns a dual quaternion, assuming current one is a transformation, which is a linear interpolation
    /// between it and a transformation provided, by a proportion also provided.<br>
    /// Note that this isn't an interpolation between dual quaternions, only between unitary dual quaternions
    /// which contains transformations.<br>
    /// To take the transformed point or vector interpolating from a transformation,
    /// its necessary to apply the resultant transformation of the lerp to the original point or vector,
    /// obtaining a point or vector in a status of transformation given by the proportion used.<br>
    /// Lerp is obtained as follows:<br>
    /// If Q1 is the original transformation, and Q2 is the transformation provided,
    /// the transformation Q' which give us the status of any point at a proportion t in [0, 1] is
    /// <br>
    /// \f$ \hat{Q} (t) = \frac{(1 - t)\hat{Q}_1 + t  \hat{Q}_2}{\left|(1 - t)\hat{Q}_1 + t  \hat{Q}_2\right|} \f$
    /// <br>
    /// Where Q'(0) = Q1 (first transformation); Q'(1) = Q2 (second transformation).
    /// </summary>
    /// <param name="fProportion">[IN] Proportion with we want to calculate the interpolation.</param>
    /// <param name="dualQuat">[IN] The other transformation implied in the interpolation.</param>
    /// <returns>
    /// The "lerped" dual quaternion.
    /// </returns>
    inline QDualQuaternion Lerp(const float_q &fProportion, const QBaseDualQuaternion &dualQuat) const
    {
        QDualQuaternion auxDualQuat = (SQFloat::_1 - fProportion) * (*this) + fProportion * rcast_q(dualQuat, const QDualQuaternion&);
        float_q fLength = auxDualQuat.GetNonDualLength();

        QE_ASSERT(fLength != SQFloat::_0)

        return auxDualQuat / fLength;
    }

    /// <summary>
    /// Converts dual quaternion into a string with the following format:<br>
    /// "DQ(r($r),d($d))".<br>
    /// Where "$" means "string representation of attribute".
    /// </summary>
    /// <returns>
    /// The std::string with the format specified.
    /// </returns>
    string_q ToString() const;

protected:

    // <summary>
    // Applies a transformation composed of a rotation and a translation, performing the rotation first and then the traslation.
    // </summary>
    // <param name="qRotation">[IN] Regular quaternion which defines the rotation.</param>
    // <param name="vTranslation">[IN] Vector which defines the translation.</param>
    // <returns>
    // The transformed dual quaternion.
    // </returns>
    template <class VectorType>
    QDualQuaternion TransformRotationFirstImp(const QBaseQuaternion &qRotation, const VectorType &vTranslation) const
    {
        QDualQuaternion rotation(qRotation, QBaseQuaternion());
        QDualQuaternion translation(QQuaternion::GetIdentity(),
                                    QBaseQuaternion(vTranslation.x * SQFloat::_0_5, vTranslation.y * SQFloat::_0_5, vTranslation.z * SQFloat::_0_5, SQFloat::_0));

        return this->Transform(translation * rotation);
    }

    // <summary>
    // Applies a transformation composed of a rotation and a translation, performing the traslation first and then the rotation.
    // </summary>
    // <param name="vTranslation">[IN] Vector which defines the translation.</param>
    // <param name="qRotation">[IN] Regular quaternion which defines the rotation.</param>
    // <returns>
    // The transformed dual quaternion.
    // </returns>
    template <class VectorType>
    QDualQuaternion TransformTranslationFirstImp(const VectorType &vTranslation, const QBaseQuaternion &qRotation) const
    {
        QDualQuaternion rotation(qRotation, QBaseQuaternion());
        QDualQuaternion translation(QQuaternion::GetIdentity(),
                                    QBaseQuaternion(vTranslation.x * SQFloat::_0_5, vTranslation.y * SQFloat::_0_5, vTranslation.z * SQFloat::_0_5, SQFloat::_0));

        return this->Transform(rotation * translation);
    }
};

} //namespace Math
} //namespace Tools
} //namespace QuimeraEngine
} //namespace Kinesis

#endif // __QDUALQUATERNION__
