// linalg.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for linear algebra helper functions

#include "../inc/quaternions.h"
#include "../inc/vect3d.h"
#include "../../../MultimodDrivers/multimod_ST7789.h"

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

// getViewRelative
// Gets the point position relative to the camera position & orientation
// (i.e. if the camera is turning to the left, the point is turning right in
// relation).
// Param Quat_t* "result": Quaternion to store the result in
// Param Quat_t* "cam_pos": Quaternion storing camera position (simpler to store as quaternion than as Vect3D_t)
// Param Quat_t* "pos": Quaternion storing position of point.
// Param Quat_t* "rot_inv": Quaternion storing the inverse of the rotation.
// Return: void
void getViewRelative(Quat_t *result, Quat_t *cam_pos, Quat_t *pos, Quat_t *rot_inv)
{
    Quat_t view_offset;

    Quat_Sub(&view_offset, cam_pos, pos);
    getRotatedQuat(result, &view_offset, rot_inv);
}
// 0, -5, -5, -110

// getViewOnScreen
// Gets the point position relative to the camera position & orientation
// (i.e. if the camera is turning to the left, the point is turning right in
// relation).
// Param Vect3D_t* "result": Result to store where the pixel will be drawn. Z is kept to determine if it is behind frame.
// Param Quat_t* "cam_frame_offset": Offset (focal length) of the projection plane.
// Param Vect3D_t* "relative_point": Relative position of a point to the camera.
// Return: void
void getViewOnScreen(Vect3D_t *result, Quat_t *cam_frame_offset, Vect3D_t *relative_point)
{
    float d, x, y, z, x_prime, y_prime;
    d = cam_frame_offset->z;
    x = relative_point->x;
    y = relative_point->y;
    z = relative_point->z;

    if (z < 0.001 && z > 0)
        z = 0.001;
    else if ((z > -0.001 && z < 0))
        z = -0.001;

    x_prime = d * x / z;
    y_prime = d * y / z;

    result->x = x_prime + X_MAX / 2;
    if (result->x < 0)
    {
        result->x = -1;
    }

    if (result->x > X_MAX)
    {
        result->x = X_MAX;
    }

    result->y = y_prime + Y_MAX / 2;
    if (result->y < 0)
    {
        result->y = -1;
    }

    if (result->y > Y_MAX)
    {
        result->y = Y_MAX;
    }

    return;
}

// interpolatePoints
// Given two points, v1, v2, interpolate the positions of points in-between.
// Param Vect3D_t* "points": array of vectors to store the interpolated pixel positions.
// Param Quat_t* "v1": vertex 1
// Param Quat_t* "v2": vertex 2
// Param uint8_t "num_points": number of points to interpolate (should always be at least 2)
// Return: void
void interpolatePoints(Vect3D_t *points, Quat_t *v1, Quat_t *v2, uint8_t num_points)
{
    float x1, x2;
    float y1, y2;
    float z1, z2;

    x1 = v1->x;
    x2 = v2->x;
    y1 = v1->y;
    y2 = v2->y;
    z1 = -v1->z;
    z2 = -v2->z;

    float slope_x = (x2 - x1) / ((float) num_points);
    float slope_y = (y2 - y1) / ((float) num_points);
    float slope_z = (z2 - z1) / ((float) num_points);

    points[0].x = x1;
    points[0].y = y1;
    points[0].z = z1;

    for (int i = 1; i < num_points; i++)
    {
        points[i].x = slope_x * i + x1;
        points[i].y = slope_y * i + y1;
        points[i].z = slope_z * i + z1;
    }

    points[num_points].x = x2;
    points[num_points].y = y2;
    points[num_points].z = z2;
}

/********************************Public Functions***********************************/
