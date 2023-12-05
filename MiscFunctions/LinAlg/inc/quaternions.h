// quaternions.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Quaternions structure & functions

#ifndef QUATERNIONS_H_
#define QUATERNIONS_H_

/************************************Includes***************************************/

#include <stdint.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/

// Quaternion definitions using Hamilton notation
typedef struct Quat_t {
    float w, x, y, z;
} Quat_t;

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

void Quat_Add(Quat_t* result, Quat_t* q1, Quat_t* q2);
void Quat_Mul(Quat_t* result, Quat_t* q1, Quat_t* q2);
void Quat_Sub(Quat_t* result, Quat_t* q1, Quat_t* q2);
void Quat_Normalize(Quat_t* q);
float Quat_GetMag(Quat_t* q);
void Quat_GetInverse(Quat_t* result, Quat_t* q);
void getRotatedQuat(Quat_t* result, Quat_t* pos, Quat_t* rot);

/********************************Public Functions***********************************/


#endif /* QUATERNIONS_H_ */

