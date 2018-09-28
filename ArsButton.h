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

#ifndef ARSBUTTON_H_INCLUDED
#define ARSBUTTON_H_INCLUDED

typedef void (*arsSimpleCallback)();

//--------------------------LOCAL INCLUDES------------------------
#include "ArsImage.h"
#include "2DObject.h"
#include "MathHelper.h"

//--------------------------GLOBAL INCLUDES-----------------------
/** texture font **/
#include <FTGL/ftgl.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

//--------------------------NAMESPACES----------------------------
using namespace std;

class ArsButtonTip
{
public:
    FTFont*  fontToUse;
    string   textToShow;
    int      fontFace;
    Vector2f displacementFromOrigin;
    Color    color;

    ArsButtonTip(string tip, Vector2f displace, FTFont* font, int fontFace, Color color);
    ~ArsButtonTip();
    void draw  (float time);
    void update(float time);
};

class ArsButton : public _2DObject
{
public:
    ArsButton (ArsImage* texture,
               b2Body * body,
               Color normal ,
               Color pressed,
               Color over   ,
               float scale);

    ~ArsButton();

    void setIcon(ArsImage* icon, float iconScale);

    /** Basic behavioral architecture methods **/
    arsSimpleCallback onClick;
    arsSimpleCallback onRelease;

    void draw  (float time);
    void update(float time);

    /** Basic b2d collisition detection interfaces **/
    void onCollisionEnter(_2DObject* collider);
    void onCollisionEnd  (_2DObject* collider);

    virtual void onPreSolve      (_2DObject* collider, const b2Manifold      * oldManifold ) {};
    virtual void onPostSolve     (_2DObject* collider, const b2ContactImpulse* impulse     ) {};

    void setTip(ArsButtonTip* tip){
        this->tip = tip;
    }

private:
    /** Tip to show when into state over **/
    ArsButtonTip* tip;

    /** Button texture image **/
    ArsImage* texture;

    /** Button icon **/
    ArsImage* icon;

    /** Colors to use in button states  **/
    Color     normal;
    Color     pressed;
    Color     over;

    /** Draw control **/
    float     iconScale;
    float     scale;
    int       state;
};

#endif // ARSBUTTON_H_INCLUDED
