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
#include "ArsButton.h"
#include "Ars.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <GL/glfw.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

//--------------------------NAMESPACES----------------------------
using namespace Ars;


ArsButtonTip::ArsButtonTip(string tip, Vector2f displace, FTFont* font, int fontFace, Color color)
{
    font->FaceSize(fontFace);
    this->color                  = color;
    this->textToShow             = tip;
    this->fontToUse              = font;
    this->displacementFromOrigin = displace;
}
//----------------------------------------------------------------------------------------------
ArsButtonTip::~ArsButtonTip()
{
}
//----------------------------------------------------------------------------------------------
void ArsButtonTip::draw  (float time)
{
    /**TODO::REFACTOR**/
    float llx, lly, llz, urx, ury, urz;

    /** Calculate font width **/
    fontToUse->BBox(textToShow.c_str(), llx, lly, llz, urx, ury, urz );

    int width  = (int) rint( urx - llx );
    int height = (int) rint( ury - lly );

    Vector2f pos(displacementFromOrigin.x, displacementFromOrigin.y);

    /** Draw font background **/
    if(textToShow.size() > 0)
    {
        drawRectangle(width + 10, height + 10, Color(0.3f,0.3f,0.3f,0.7f), Vector2f(displacementFromOrigin.x - 5,displacementFromOrigin.y - 5), 1.0f);
    }
    /**TODO::REFACTOR**/

    glPushMatrix();
    glTranslatef(pos.x, pos.y, 0);
    glColor4f(color.r,color.g,color.b,color.a);
    fontToUse->Render(textToShow.c_str());
    glPopMatrix();
}
//----------------------------------------------------------------------------------------------
void ArsButtonTip::update(float time)
{
}
//----------------------------------------------------------------------------------------------
ArsButton::ArsButton(ArsImage* texture,
                     b2Body * body,
                     Color normal ,
                     Color pressed,
                     Color over   ,
                     float scale)
{
    this->texture = texture;
    this->normal  = normal;
    this->pressed = pressed;
    this->over    = over;
    this->scale   = scale;

    /** Create b2D body **/
    rigidbody = body;
    tag       = BUTTON;
    state     = ARS_BTN_NORMAL;
    icon      = NULL;

    onClick   = NULL;
    onRelease = NULL;
    tip       = NULL;
}
//----------------------------------------------------------------------------------------------
ArsButton::~ArsButton()
{

}
//----------------------------------------------------------------------------------------------
void ArsButton::draw(float time)
{
    switch(state)
    {
    case ARS_BTN_NORMAL:
        Ars::drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), normal, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
        if(icon!=NULL)
        {
            Ars::drawImage(icon->getId(), icon->getWidth(), icon->getHeight(), normal, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, iconScale);
        }
        break;
    case ARS_BTN_PRESSED:
        Ars::drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), pressed, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
        if(icon!=NULL)
        {
            Ars::drawImage(icon->getId()   , icon->getWidth()   , icon->getHeight()   , pressed, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, iconScale);
        }
        break;
    case ARS_BTN_OVER:
        Ars::drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), over, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
        if(icon!=NULL)
        {
            Ars::drawImage(icon->getId()   , icon->getWidth()   , icon->getHeight()   , over, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, iconScale);
        }
        break;
    }

    if(tip != NULL && state == ARS_BTN_OVER)
    {
        tip->draw(0);
    }
}
//----------------------------------------------------------------------------------------------
/** Button behavior **/
void ArsButton::update(float time)
{
    switch(state)
    {
    case ARS_BTN_NORMAL:
        break;
    case ARS_BTN_PRESSED:
        if(Ars::_mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]          == GLFW_RELEASE &&
                Ars::_mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT]  != GLFW_RELEASE)
        {
            state = ARS_BTN_OVER;
            if(onRelease != NULL)
            {
                onRelease();
            }
        }

        break;
    case ARS_BTN_OVER:
        if(Ars::_mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]         == GLFW_PRESS &&
                Ars::_mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_PRESS)
        {
            state = ARS_BTN_PRESSED;
            if(onClick != NULL)
            {
                onClick();
            }
        }

        if(Ars::_mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]          == GLFW_RELEASE &&
                Ars::_mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT]  != GLFW_RELEASE)
        {

        }
        break;
    }
}
//----------------------------------------------------------------------------------------------
void ArsButton::setIcon(ArsImage* icon, float iconScale)
{
    this->icon      = icon;
    this->iconScale = iconScale;
}
//----------------------------------------------------------------------------------------------
void ArsButton::onCollisionEnter(_2DObject* collider)
{
    if(collider->tag == MOUSE)
    {
        state = ARS_BTN_OVER;
    }
}
//----------------------------------------------------------------------------------------------
void ArsButton::onCollisionEnd(_2DObject* collider)
{
    if(collider->tag == MOUSE)
    {
        state = ARS_BTN_NORMAL;
    }
}
