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

#ifndef MATHHELPER_H
#define MATHHELPER_H

//--------------------------LOCAL INCLUDES------------------------

//--------------------------GLOBAL INCLUDES-----------------------
#include <math.h>
#include <cmath>
//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------


/*
 * Vector2f class - Can be used to store coordinates values from a 2D space.
 *
 * version
 *		1.0 25 August 2014
 *
 * author
 *		Alysson Silva
 */
class Vector2f
{
public:
    float x; // Store X coordinate value
    float y; // Store Y coordinate value
    float z; // Store Z coordinate value
    float w; // Store W coordinate value

    //float len;

    /**
     * Default constructor, can be used to create an instance of this object.
     */
    Vector2f()
    {
        //len = 0.0f;
        x   = 0.0f;
        y   = 0.0f;
        z   = 0.0f;
        w   = 0.0f;
    }

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
        z = 0.0f;
        w = 0.0f;
    }

    Vector2f operator + (Vector2f& value)
    {
        Vector2f res;

        res.x = x + value.x;
        res.y = y + value.y;

        return res;
    }

    Vector2f operator - (Vector2f& value)
    {
        Vector2f res;

        res.x = x - value.x;
        res.y = y - value.y;

        return res;
    }

    Vector2f operator * (float scalar)
    {
        Vector2f res;

        res.x = x * scalar;
        res.y = y * scalar;

        return res;
    }

    Vector2f operator = (Vector2f value)
    {
        x = value.x;
        y = value.y;

        return *this;
    }

    float lenght()
    {
        return (float)sqrt(pow(x,2.0) + pow(y, 2.0));
    }

    void normalize()
    {
        float len = lenght();
        x = x / len;
        y = y / len;
    }
};

/*
 * Vector3f class - Can be used to store coordinates values from a 3D space.
 *
 * version
 *		1.0 25 August 2014
 *
 * author
 *		Alysson Silva
 */
class Vector3f
{
public:
    float x; // Store X coordinate value
    float y; // Store Y coordinate value
    float z; // Store Z coordinate value
    float w; // Store W coordinate value

    //float len;

    /**
     * Default constructor, can be used to create an instance of this object.
     */
    Vector3f()
    {
        //len = 0.0f;
        x   = 0.0f;
        y   = 0.0f;
        z   = 0.0f;
        w   = 0.0f;
    }

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
        w = 0.0f;
    }

    Vector3f operator + (Vector3f& value)
    {
        Vector3f res;

        res.x = x + value.x;
        res.y = y + value.y;
        res.z = z + value.z;

        return res;
    }

    Vector3f operator - (Vector3f& value)
    {
        Vector3f res;

        res.x = x - value.x;
        res.y = y - value.y;
        res.z = z - value.z;

        return res;
    }

    Vector3f operator * (float scalar)
    {
        Vector3f res;

        res.x = x * scalar;
        res.y = y * scalar;
        res.z = z * scalar;

        return res;
    }

    Vector3f operator = (Vector3f value)
    {
        x = value.x;
        y = value.y;
        z = value.z;

        return *this;
    }

    float lenght()
    {
        return (float)sqrt(pow(x,2.0) + pow(y, 2.0) + pow(z, 2.0));
    }

    void normalize()
    {
        float len = lenght();
        x = x / len;
        y = y / len;
        z = z / len;
    }
};

float dist(Vector3f& A, Vector3f& B);

Vector2f cross(Vector2f& A, Vector2f& B);
float    dot  (Vector2f& A, Vector2f& B);

Vector3f cross(Vector3f& A, Vector3f& B);
float    dot  (Vector3f& A, Vector3f& B);


class Color
{
public:
    float r;
    float g;
    float b;
    float a;

    Color()
    {
        r = 0.0f;
        g = 0.0f;
        b = 0.0f;
        a = 1.0f;
    }

    Color(float red, float green, float blue, float alpha)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
};

/*
 * Rectangle class - Can be used to store coordinates of rectangle points into a 3D space.
 *
 * version
 *		1.0 25 August 2014
 *
 * author
 *		Alysson Silva
 */
class Rectangle
{
public:
    Vector2f min; // Store min coordinates values
    Vector2f max; // Store max coordinates values

    /**
     * Default constructor, can be used to create an instance of this object.
     */
    Rectangle();
};

#endif
