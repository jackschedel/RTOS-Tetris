// vect3d.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for 3D vectors

#include "../inc/vect3d.h"

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
bool Vect3D_Equal(Vect3D_t *v1, Vect3D_t *v2)
{
    if (v1->x != v2->x)
        return false;

    if (v1->y != v2->y)
        return false;

    if (v1->z != v2->z)
        return false;

    return true;
}

void Vect3D_Add(Vect3D_t *result, Vect3D_t *v1, Vect3D_t *v2)
{
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;

    return;
}

void Vect3D_Mul(Vect3D_t *result, Vect3D_t *v1, Vect3D_t *v2)
{
    result->x = (v1->y * v2->z) - (v1->z * v2->y);
    result->y = (v1->z * v2->x) - (v1->x * v2->z);
    result->z = (v1->x * v2->y) - (v1->y * v2->x);

    return;
}

void Vect3D_Sub(Vect3D_t *result, Vect3D_t *v1, Vect3D_t *v2)
{
    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
    result->z = v1->z - v2->z;

    return;
}

void Vect3D_Normalize(Vect3D_t *v)
{
    float mag = Vect3D_GetMag(v);

    // prevents division by 0
    if (mag < 0.001)
    {
        mag = 0.001;
    }

    v->x /= mag;
    v->y /= mag;
    v->z /= mag;

    return;
}

float Vect3D_GetMag(Vect3D_t *v)
{
    return sqrtf(powf(v->x, 2.0) + powf(v->y, 2.0) + powf(v->z, 2.0));
}

void Vect3D_GetInverse(Vect3D_t *result, Vect3D_t *v)
{
    result->x = -(v->x);
    result->y = -(v->y);
    result->z = -(v->z);

    return;
}

/********************************Public Functions***********************************/
