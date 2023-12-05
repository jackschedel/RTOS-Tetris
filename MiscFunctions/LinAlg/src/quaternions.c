// quaternions.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for quaternions for rotation definitions

#include "../inc/quaternions.h"

/************************************Includes***************************************/

#include <math.h>

/************************************Includes***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// Quat_Add
// Adds two quaternions together. Only meaningful and returns the composite rotation
// iff the result is infintisemal. i.e. if the rotation is really small!
// This could be useful is speed is a priority, especially for smaller rotations.
// Param "result": Pointer to result quaternions
// Param "q1": Points to left quaternion
// Param "q2": Pointer to right quaternion
// Return: void
void Quat_Add(Quat_t *result, Quat_t *q1, Quat_t *q2)
{
    result->w = q1->w + q2->w;
    result->x = q1->x + q2->x;
    result->y = q1->y + q2->y;
    result->z = q1->z + q2->z;

    return;
}

// Quat_Mul
// Multiplies two quaternions together, obtaining rotation.
// Quaternion multiplication is NOT commutative. Order matters!
// Param "result": Pointer to result quaternions
// Param "q1": Points to left quaternion
// Param "q2": Pointer to right quaternion
// Return: void
void Quat_Mul(Quat_t *result, Quat_t *q1, Quat_t *q2)
{

    result->x = (q1->w * q2->x) + (q1->x * q2->w) + (q1->y * q2->z) - (q1->z * q2->y);
    result->y = (q1->w * q2->y) - (q1->x * q2->z) + (q1->y * q2->w) + (q1->z * q2->x);
    result->z = (q1->w * q2->z) + (q1->x * q2->y) - (q1->y * q2->x) + (q1->z * q2->w);

    return;
}

// Quat_Add
// Subtracts two quaternions. Only meaningful and returns the composite rotation
// iff the result is infintisemal. i.e. if the rotation is really small!
// This could be useful is speed is a priority, especially for smaller rotations.
// Param "result": Pointer to result quaternions
// Param "q1": Points to left quaternion
// Param "q2": Pointer to right quaternion
// Return: void
void Quat_Sub(Quat_t *result, Quat_t *q1, Quat_t *q2)
{
    result->w = q1->w - q2->w;
    result->x = q1->x - q2->x;
    result->y = q1->y - q2->y;
    result->z = q1->z - q2->z;

    return;
}

// Quat_Normalize
// Quaternion magnitudes greater than 1 are unstable and can cause unintended
// behavior.
// Param "q": Pointer to quaternion
// Return: void
void Quat_Normalize(Quat_t *q)
{
    float mag = Quat_GetMag(q);

    // prevents division by 0
    if (mag < 0.001)
    {
        mag = 0.001;
    }

    q->w /= mag;
    q->x /= mag;
    q->y /= mag;
    q->z /= mag;

    return;
}

// Quat_GetMag
// Gets magnitude of the quaternion
// Param "q": Pointer to quaternion
// Return: void
float Quat_GetMag(Quat_t *q)
{
    return sqrtf(powf(q->w, 2.0) + powf(q->x, 2.0) + powf(q->y, 2.0) + powf(q->z, 2.0));
}

// Quat_GetMag
// Gets inverse  of the quaternion
// Param "q": Pointer to quaternion
// Return: void
void Quat_GetInverse(Quat_t *result, Quat_t *q)
{
    result->w = q->w;
    result->x = -(q->x);
    result->y = -(q->y);
    result->z = -(q->z);

    return;
}

// getRotatedQuat
// Rotates quaternion based on another rotation quaternion.
// If this is an offset of an original position, rotating it
// and then adding to its original position gets you the rotated point
// in relation to the rotation of the original point it is offset from.
// Param "result": Pointer to result quaternion container
// Param "pos": Pointer to quaternion that holds position
// Param "rot": Pointer to rotation quaternion to rotate by
// Return: void
void getRotatedQuat(Quat_t *result, Quat_t *pos, Quat_t *rot)
{
    Quat_t inverted_rot, temp_quat_mult;
    Quat_GetInverse(&inverted_rot, rot);

    // quaternion rotation: rot * pos * rot^-1
    Quat_Mul(&temp_quat_mult, rot, pos);
    Quat_Mul(result, &temp_quat_mult, &inverted_rot);

    return;
}

/********************************Public Functions***********************************/
