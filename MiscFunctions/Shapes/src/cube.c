// cube->h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Cube

#ifndef CUBE_H_
#define CUBE_H_

/************************************Includes***************************************/

#include "../inc/cube.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// Cube_Generate
// Generates position of cube vertices using a predefined cube structure
// Param Quat_t* "v": array of vertices to store the generated points.
// Param cube_t* "cube": Cube struct containing already defined x_pos, y_pos, z_pos, and w, h, l.
// Return: void
void Cube_Generate(Quat_t* v, cube_t* cube) {
    v[0].w = 0;
    v[0].x = cube->x_pos + 0;
    v[0].y = cube->y_pos + 0;
    v[0].z = cube->z_pos + 0;

    v[1].w = 0;
    v[1].x = cube->x_pos + cube->width;
    v[1].y = cube->y_pos + 0;
    v[1].z = cube->z_pos + 0;

    v[2].w = 0;
    v[2].x = cube->x_pos + cube->width;
    v[2].y = cube->y_pos + cube->height;
    v[2].z = cube->z_pos + 0;

    v[3].w = 0;
    v[3].x = cube->x_pos + 0;
    v[3].y = cube->y_pos + cube->height;
    v[3].z = cube->z_pos + 0;

    v[4].w = 0;
    v[4].x = cube->x_pos + 0;
    v[4].y = cube->y_pos + 0;
    v[4].z = cube->z_pos + cube->length;

    v[5].w = 0;
    v[5].x = cube->x_pos + cube->width;
    v[5].y = cube->y_pos + 0;
    v[5].z = cube->z_pos + cube->length;

    v[6].w = 0;
    v[6].x = cube->x_pos + cube->width;
    v[6].y = cube->y_pos + cube->height;
    v[6].z = cube->z_pos + cube->length;

    v[7].w = 0;
    v[7].x = cube->x_pos + 0;
    v[7].y = cube->y_pos + cube->height;
    v[7].z = cube->z_pos + cube->length;
}

/********************************Public Functions***********************************/


#endif /* CUBE_H_ */


