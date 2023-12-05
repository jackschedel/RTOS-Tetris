// vect3d.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// 3d vector declarations and structure

#ifndef VECT3D_H_
#define VECT3D_H_

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/

// Vector x, y, z
typedef struct Vect3D_t
{
    float x, y, z;
} Vect3D_t;

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/
bool Vect3D_Equal(Vect3D_t *v1, Vect3D_t *v2);

void Vect3D_Add(Vect3D_t *result, Vect3D_t *v1, Vect3D_t *v2);
void Vect3D_Mul(Vect3D_t *result, Vect3D_t *v1, Vect3D_t *v2);
void Vect3D_Sub(Vect3D_t *result, Vect3D_t *v1, Vect3D_t *v2);
void Vect3D_Normalize(Vect3D_t *v);
float Vect3D_GetMag(Vect3D_t *v);
void Vect3D_GetInverse(Vect3D_t *result, Vect3D_t *v);

/********************************Public Functions***********************************/

#endif /* VECT3D_H_ */

