/*MIT License
*
*Copyright (c) 2018 Alysson Ribeiro da Silva
*
*Permission is hereby granted, free of charge, to any person obtaining a copy 
*of this software and associated documentation files (the "Software"), to deal 
*in the Software without restriction, including *without limitation the rights 
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
*copies of the Software, and to permit persons to whom the Software is furnished 
*to do so, subject *to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *IN NO EVENT SHALL THE AUTHORS 
*OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN 
*AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
*THE *SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//--------------------------LOCAL INCLUDES------------------------
#include "MathHelper.h"

//--------------------------GLOBAL INCLUDES-----------------------

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------


float dist(Vector3f& A, Vector3f& B){
    float dist;

    float x =  A.x - B.x;
    float y =  A.y - B.y;
    float z =  A.z - B.z;


    dist = (float)sqrt(pow(x,2.0) + pow(y, 2.0) + pow(z, 2.0));

    return std::abs(dist);
}
//----------------------------------------------------------------------------------------------
Vector3f cross(Vector3f& A, Vector3f& B){
    Vector3f result;

    result.x = A.y*B.z - A.z*B.y;
    result.y = A.z*B.x - A.x*B.z;
    result.z = A.x*B.y - A.y*B.x;

    return result;
}
//----------------------------------------------------------------------------------------------
float dot(Vector3f& A, Vector3f& B){
    float result;

    result = A.x*B.x + A.y*B.y + A.z*B.z;

    return result;
}
//----------------------------------------------------------------------------------------------
Vector2f cross(Vector2f& A, Vector2f& B){
    Vector2f result;

    result.x =      A.x*B.y;
    result.y = -1 * B.x*A.y;

    return result;
}
//----------------------------------------------------------------------------------------------
float dot(Vector2f& A, Vector2f& B){
    float result;

    result = A.x*B.x + A.y*B.y;

    return result;
}

