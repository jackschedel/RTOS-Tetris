// linalg.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Linear algebra library for dealing with quaternion manipulation

#ifndef LINALG_H_
#define LINALG_H_

/************************************Includes***************************************/

#include <math.h>
#include "../inc/quaternions.h"
#include "../inc/vect3d.h"
#include <stdint.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

void getViewRelative(Quat_t *result, Quat_t *cam_pos, Quat_t *pos, Quat_t *rot_inv);
void getViewOnScreen(Vect3D_t *result, Quat_t *cam_frame_offset, Vect3D_t *point);
void interpolatePoints(Vect3D_t *points, Quat_t *v1, Quat_t *v2, uint8_t num_points);

/********************************Public Functions***********************************/

#endif /* LINALG_H_ */

