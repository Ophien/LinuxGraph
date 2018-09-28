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
#include "Ars.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <GLFW/glfw3.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

using namespace Ars;
using namespace std;

ArsVertex::ArsVertex(ArsImage* texture,
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

    //Gera ID para o vertice
    ID = Ars::genVertexId();

    /** Initial state of the edges **/
    for(int i= 0; i < ARS_TOTAL_VERTEX; i++)
    {
        edges        [i] = -1;
        edgesDirected[i] = -1;
    }

    drawed    = false;
    marked    = false;
    selected  = false;
    labelV    = "";
}
//----------------------------------------------------------------------------------------------
ArsVertex::~ArsVertex()
{

}
//----------------------------------------------------------------------------------------------
int ArsVertex::getId()
{
    return ID;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::preDraw(float time)
{
    /** Edge color **/
    Color outColor;
    Color inColor;
    bool  draw = true;

    /** Draw all edges **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        /** Optimizations on draw operations **/
        if(_drawArrow)
        {
            if(!hasEdgeTo(i, _drawArrow))
            {
                if(_sceneVertex[i] != NULL)
                {
                    if(!_sceneVertex[i]->hasEdgeTo(ID,_drawArrow))
                        continue;
                }
                else
                    continue;
            }
        }
        else
        {
            if(!hasEdgeTo(i, _drawArrow))
                continue;
        }

        /** Optimizations on draw operations **/
        if(_sceneVertex[i]->isDrawed() && !(selected || marked))
            continue;

        /** Optimizations on draw operations **/
        if(_sceneVertex[i]->isSelected())
            continue;

        if(selected || marked)
        {
            outColor = Color(0.4f,0.8f,0.4f,1.0f);
            inColor  = Color(0.9f,0.2f,0.2f,1.0f);
        }
        else
        {
            // if(!_drawArrow && _sceneVertex[i]->isSelected())
            outColor = Color(0.0f,0.0f,0.0f,1.0f);
        }

        if(_separateEdges && (_opVertex != NULL || _marks > 0))
        {
            if(!selected && !marked)
                draw = false;
        }

        if(draw)
        {
            if(_drawArrow)
            {
                if(hasEdgeTo(i, _drawArrow) && _sceneVertex[i] != NULL)
                    drawArrow(rigidbody->GetPosition(), _sceneVertex[i]->rigidbody->GetPosition(), outColor);

                if(_sceneVertex[i] != NULL)
                    if(_sceneVertex[i]->hasEdgeTo(ID,_drawArrow))
                    {
                        drawArrow(_sceneVertex[i]->rigidbody->GetPosition(), rigidbody->GetPosition(), inColor);
                    }
            }
            else
            {
                Vector3f outVert(rigidbody->GetPosition().x,rigidbody->GetPosition().y,0);
                Vector3f inVert (_sceneVertex[i]->rigidbody->GetPosition().x,_sceneVertex[i]->rigidbody->GetPosition().y,0);

                drawLine(outVert, inVert, outColor, 3);
            }
        }
    }//END FOR EDGES

    markDrawed(true);
}
//----------------------------------------------------------------------------------------------
void ArsVertex::posDraw(float time)
{
    /** Label color **/
    Color outColor;
    Color inColor;
    bool  draw = true;

    /** Draw vertex label **/
    if(_disableLabelsB)
    {
        string renderStr = labelV;

        if(_labelB && state == ARS_BTN_PRESSED)
        {
            renderStr += "/";
        }

        float llx, lly, llz, urx, ury, urz;

        /** Calculate font width **/
        _font->BBox(renderStr.c_str(), llx, lly, llz, urx, ury, urz );

        int width  = (int) rint( urx - llx );
        int height = (int) rint( ury - lly );

        Vector2f pos(rigidbody->GetPosition().x - width/2, rigidbody->GetPosition().y + 30);

        /** Draw font background **/
        if(renderStr.size() > 0)
        {
            drawRectangle(width + 10, height + 10, Color(0.0f,0.0f,0.0f,0.7f), Vector2f(pos.x-5,pos.y-5), 1.0f);
        }

        glPushMatrix();
        glColor3f(1.0f,1.0f,1.0f);
        glTranslatef(pos.x, pos.y, 0);
        _font->Render(renderStr.c_str());
        glPopMatrix();
    }

    /** Draw all labels **/
    if(_disableEdgeWeight)
        return;

    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        /** Optimizations on draw operations **/
        if(_drawArrow)
        {
            if(!hasEdgeTo(i, _drawArrow))
            {
                if(_sceneVertex[i] != NULL)
                {
                    if(!_sceneVertex[i]->hasEdgeTo(ID,_drawArrow))
                        continue;
                }
                else
                    continue;
            }
        }
        else
        {
            if(!hasEdgeTo(i, _drawArrow))
                continue;
        }

        /** Optimizations on draw operations **/
        if(_sceneVertex[i]->isDrawed() && !(selected || marked))
            continue;

        /** Optimizations on draw operations **/
        if(_sceneVertex[i]->isSelected())
            continue;

        Vector3f outVert(0,0,0);
        Vector3f inVert (0,0,0);

        if(selected || marked)
        {
            outColor = Color(0.4f,0.8f,0.4f,1.0f);
            inColor  = Color(0.9f,0.2f,0.2f,1.0f);
        }
        else
        {
            // if(!_drawArrow && _sceneVertex[i]->isSelected())
            outColor = Color(1.0f,1.0f,0.0f,1.0f);
        }

        if(_separateEdges && (_opVertex != NULL || _marks > 0))
        {
            if(!selected && !marked)
                draw = false;
        }

        /**----------------------------------------------!!! REFACTOR THIS SECTION !!! ----------------------------------------------**/
        if(draw)
        {
            if(_currentOp == ARS_LABEL_EDGE && _out != NULL && _in != NULL)
            {
                if(_out->getId() == ID && i == _in->getId())
                {
                    outVert.x = rigidbody->GetPosition().x;
                    outVert.y = rigidbody->GetPosition().y;
                    inVert .x = _sceneVertex[i]->rigidbody->GetPosition().x;
                    inVert .y = _sceneVertex[i]->rigidbody->GetPosition().y;

                    Vector3f labelPos = outVert;
                    Vector3f edgeVec  = inVert - outVert;
                    float edgeLenght  = edgeVec.lenght();
                    edgeVec.normalize();
                    edgeVec           = edgeVec * (edgeLenght / 2.0f);

                    /** Ensure label positioning for directed graph **/
                    Vector3f vPerp1(-edgeVec.y, edgeVec.x, 0);
                    Vector3f v1 = edgeVec + vPerp1;
                    v1.normalize();
                    v1 = v1 * 30;

                    if(!_drawArrow)
                        labelPos  = labelPos + edgeVec;
                    else
                        labelPos  = labelPos + edgeVec + v1;

                    string renderStr = to_string(getEdgeWeight(i, _drawArrow));
                    renderStr += "/";

                    /**TODO::REFACTOR**/
                    float llx, lly, llz, urx, ury, urz;

                    /** Calculate font width **/
                    _font->BBox(renderStr.c_str(), llx, lly, llz, urx, ury, urz );

                    int width  = (int) rint( urx - llx );
                    int height = (int) rint( ury - lly );

                    Vector2f pos(labelPos.x - width/2, labelPos.y);

                    /** Draw font background **/
                    if(draw && renderStr.size() > 0)
                    {
                        drawRectangle(width + 10, height + 10, Color(0.0f,0.0f,0.0f,0.7f), Vector2f(pos.x - 5,pos.y - 5), 1.0f);
                    }
                    /**TODO::REFACTOR**/

                    glPushMatrix();
                    glTranslatef(pos.x, pos.y, 0);
                    glColor4f(outColor.r,outColor.g,outColor.b,outColor.a);
                    _font->Render(renderStr.c_str());
                    glPopMatrix();
                }
                else if(_out->getId() == i && ID == _in->getId())
                {
                    outVert.x = rigidbody->GetPosition().x;
                    outVert.y = rigidbody->GetPosition().y;
                    inVert .x = _sceneVertex[i]->rigidbody->GetPosition().x;
                    inVert .y = _sceneVertex[i]->rigidbody->GetPosition().y;

                    Vector3f labelPosIN = inVert;
                    Vector3f edgeVec  = outVert - inVert;
                    float edgeLenght  = edgeVec.lenght();
                    edgeVec.normalize();
                    edgeVec           = edgeVec * (edgeLenght / 2.0f);

                    /** Ensure label positioning for directed graph **/
                    Vector3f vPerp1(-edgeVec.y, edgeVec.x, 0);
                    Vector3f v1 = edgeVec + vPerp1;
                    v1.normalize();
                    v1 = v1 * 30;

                    if(!_drawArrow)
                        labelPosIN  = labelPosIN + edgeVec;
                    else
                        labelPosIN  = labelPosIN + edgeVec + v1;

                    string renderStr = to_string(_sceneVertex[i]->getEdgeWeight(ID, _drawArrow));
                    renderStr += "/";

                    /**TODO::REFACTOR**/
                    float llx, lly, llz, urx, ury, urz;

                    /** Calculate font width **/
                    _font->BBox(renderStr.c_str(), llx, lly, llz, urx, ury, urz );

                    int width  = (int) rint( urx - llx );
                    int height = (int) rint( ury - lly );

                    Vector2f posIN(labelPosIN.x - width/2, labelPosIN.y);

                    /** Draw font background **/
                    if(renderStr.size() > 0)
                    {
                        drawRectangle(width + 10, height + 10, Color(0.0f,0.0f,0.0f,0.7f), Vector2f(posIN.x - 5,posIN.y - 5), 1.0f);
                    }
                    /**TODO::REFACTOR**/

                    glPushMatrix();
                    glTranslatef(posIN.x, posIN.y, 0);
                    if(selected)
                        glColor4f(inColor.r,inColor.g,inColor.b,inColor.a);
                    else
                        glColor4f(outColor.r,outColor.g,outColor.b,outColor.a);
                    _font->Render(renderStr.c_str());
                    glPopMatrix();
                }
            }
            else
            {
                if(getEdgeWeight(i, _drawArrow) > 0)
                {
                    outVert.x = rigidbody->GetPosition().x;
                    outVert.y = rigidbody->GetPosition().y;
                    inVert .x = _sceneVertex[i]->rigidbody->GetPosition().x;
                    inVert .y = _sceneVertex[i]->rigidbody->GetPosition().y;

                    Vector3f labelPos = outVert;
                    Vector3f edgeVec  = inVert - outVert;
                    float edgeLenght  = edgeVec.lenght();
                    edgeVec.normalize();
                    edgeVec           = edgeVec * (edgeLenght / 2.0f);

                    /** Ensure label positioning for directed graph **/
                    Vector3f vPerp1(-edgeVec.y, edgeVec.x, 0);
                    Vector3f v1 = edgeVec + vPerp1;
                    v1.normalize();
                    v1 = v1 * 30;

                    if(!_drawArrow)
                        labelPos  = labelPos + edgeVec;
                    else
                        labelPos  = labelPos + edgeVec + v1;

                    string renderStr = to_string(getEdgeWeight(i, _drawArrow));

                    /**TODO::REFACTOR**/
                    float llx, lly, llz, urx, ury, urz;

                    /** Calculate font width **/
                    _font->BBox(renderStr.c_str(), llx, lly, llz, urx, ury, urz );

                    int width  = (int) rint( urx - llx );
                    int height = (int) rint( ury - lly );

                    Vector2f pos(labelPos.x - width/2, labelPos.y);

                    /** Draw font background **/
                    if(draw && renderStr.size() > 0)
                    {
                        drawRectangle(width + 10, height + 10, Color(0.0f,0.0f,0.0f,0.7f), Vector2f(pos.x - 5,pos.y - 5), 1.0f);
                    }
                    /**TODO::REFACTOR**/

                    glPushMatrix();
                    glTranslatef(pos.x, pos.y, 0);
                    glColor4f(outColor.r,outColor.g,outColor.b,outColor.a);
                    _font->Render(renderStr.c_str());
                    glPopMatrix();
                }

                if(_drawArrow)
                {
                    if(_sceneVertex[i]->getEdgeWeight(ID, _drawArrow) > 0)
                    {
                        outVert.x = rigidbody->GetPosition().x;
                        outVert.y = rigidbody->GetPosition().y;
                        inVert .x = _sceneVertex[i]->rigidbody->GetPosition().x;
                        inVert .y = _sceneVertex[i]->rigidbody->GetPosition().y;

                        Vector3f labelPosIN = inVert;
                        Vector3f edgeVec  = outVert - inVert;
                        float edgeLenght  = edgeVec.lenght();
                        edgeVec.normalize();
                        edgeVec           = edgeVec * (edgeLenght / 2.0f);

                        /** Ensure label positioning for directed graph **/
                        Vector3f vPerp1(-edgeVec.y, edgeVec.x, 0);
                        Vector3f v1 = edgeVec + vPerp1;
                        v1.normalize();
                        v1 = v1 * 30;

                        if(!_drawArrow)
                            labelPosIN  = labelPosIN + edgeVec;
                        else
                            labelPosIN  = labelPosIN + edgeVec + v1;

                        string renderStr = to_string(_sceneVertex[i]->getEdgeWeight(ID, _drawArrow));

                        /**TODO::REFACTOR**/
                        float llx, lly, llz, urx, ury, urz;

                        /** Calculate font width **/
                        _font->BBox(renderStr.c_str(), llx, lly, llz, urx, ury, urz );

                        int width  = (int) rint( urx - llx );
                        int height = (int) rint( ury - lly );

                        Vector2f posIN(labelPosIN.x - width/2, labelPosIN.y);

                        /** Draw font background **/
                        if(renderStr.size() > 0)
                        {
                            drawRectangle(width + 10, height + 10, Color(0.0f,0.0f,0.0f,0.7f), Vector2f(posIN.x - 5,posIN.y - 5), 1.0f);
                        }
                        /**TODO::REFACTOR**/

                        glPushMatrix();
                        glTranslatef(posIN.x, posIN.y, 0);
                        if(selected)
                            glColor4f(inColor.r,inColor.g,inColor.b,inColor.a);
                        else
                            glColor4f(outColor.r,outColor.g,outColor.b,outColor.a);
                        _font->Render(renderStr.c_str());
                        glPopMatrix();
                    }
                }//END IF DRAW ARROW
            }
        }//END IF DRAW
        /**----------------------------------------------!!! REFACTOR THIS SECTION !!! ----------------------------------------------**/
    }//END FOR EDGES

    markDrawed(true);
}
//----------------------------------------------------------------------------------------------
void ArsVertex::draw(float time)
{
    if(enable)
    {
        if(marked){
            drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), pressed, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
            return;
        }

        /** Draw all edges here **/
        switch(state)
        {
        case ARS_BTN_NORMAL:
            drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), normal, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
            if(icon!=NULL)
            {
                drawImage(icon->getId(), icon->getWidth(), icon->getHeight(), normal, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, iconScale);
            }
            break;
        case ARS_BTN_PRESSED:
            drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), pressed, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
            if(icon!=NULL)
            {
                drawImage(icon->getId()   , icon->getWidth()   , icon->getHeight()   , pressed, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, iconScale);
            }
            break;
        case ARS_BTN_OVER:
            drawImage(texture->getId(), texture->getWidth(), texture->getHeight(), over, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, scale);
            if(icon!=NULL)
            {
                drawImage(icon->getId()   , icon->getWidth()   , icon->getHeight()   , over, Vector2f(rigidbody->GetPosition().x, rigidbody->GetPosition().y), ARS_CENTER, iconScale);
            }
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------
bool ArsVertex::isSelected()
{
    return selected;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::setLabel(const char* label)
{
    this->labelV = label;
}
//----------------------------------------------------------------------------------------------
string ArsVertex::getLabel()
{
    return labelV;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::insertEdge      (int in, int weight, bool direct)
{
    //if(!hasEdgeTo(in,direct))
   // {
        if(direct){
            edgesDirected[in] = weight;
            if(_sceneVertex[in] != NULL)
            {
                if(_sceneVertex[in]->hasEdgeTo(ID, _drawArrow))
                {
                    _sceneVertex [in]->setNonEdgeWeight(ID, weight);
                    edges        [in] = weight;
                }
            }
        }else
        {
            _sceneVertex [in]->setNonEdgeWeight(ID, weight);
            edges        [in] = weight;
            edgesDirected[in] = weight;
            _sceneVertex [in]->setDirectedEdgeWeight(ID, weight);
        }
    //}
}
//----------------------------------------------------------------------------------------------
bool ArsVertex::hasEdgeTo (int in, bool direct)
{
    bool resp = false;

    if(direct)
    {
        if(edgesDirected[in] >= 0)
            resp = true;
    }
    else if(edges[in] >= 0)
        resp = true;

    return resp;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::setDirectedEdgeWeight(int in, int weight)
{
    edgesDirected[in] = weight;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::setNonEdgeWeight     (int in, int weight)
{
    edges[in] = weight;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::setEdgeWeight(int in, int weight, bool direct)
{
    if(direct)
        edgesDirected[in] = weight;
    else
    {
        _sceneVertex [in]->setNonEdgeWeight(ID, weight);
        edges        [in] = weight;
        edgesDirected[in] = weight;
        _sceneVertex [in]->setDirectedEdgeWeight(ID, weight);
    }
}
//----------------------------------------------------------------------------------------------
int ArsVertex::getEdgeWeight    (int in, bool direct)
{
    if(direct)
        return edgesDirected[in];
    else
        return edges[in];
}
//----------------------------------------------------------------------------------------------
int ArsVertex::getState()
{
    return state;
}
//----------------------------------------------------------------------------------------------
/** Button behavior **/
void ArsVertex::update(float time)
{
    if(enable)
    {
        if(_usePhysics)
        {
            rigidbody->SetLinearVelocity(b2Vec2(0.0f,0.0f));
            //   rigidbody->SetGravityScale         (0.0f);
        }
        else
        {
        }

        switch(state)
        {
        case ARS_BTN_NORMAL:

            break;
        case ARS_BTN_PRESSED:
            if(_mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]               == GLFW_RELEASE &&
                    _mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT]  != GLFW_RELEASE)
            {
                if(_currentOp == ARS_MOVE_VERTEX)
                {
                    state    = ARS_BTN_OVER;
                    _opVertex = NULL;
                    selected = false;

                    /** Destroy mouse joint **/
                   /* _world->DestroyJoint(_mouseJointE);
                    _mouseJointE = NULL;*/

                    if(onRelease != NULL)
                    {
                        onRelease();
                    }
                }
            }

            if(_currentOp == ARS_MOVE_VERTEX)
            {
                //if(_mouseJointE != NULL){
                //    _mouseJointE->SetTarget(b2Vec2(_mousePosition2D_world.x, _mousePosition2D_world.y));
                //}
                rigidbody->SetTransform(b2Vec2(_mousePosition2D_world.x, _mousePosition2D_world.y), 0.0f);

                if(rigidbody->GetPosition().x < 0)
                    rigidbody->SetTransform(b2Vec2(0, rigidbody->GetPosition().y), 0.0f);

                if(rigidbody->GetPosition().x > _windowWidth)
                    rigidbody->SetTransform(b2Vec2(_windowWidth, rigidbody->GetPosition().y), 0.0f);

                if(rigidbody->GetPosition().y < 0)
                    rigidbody->SetTransform(b2Vec2(rigidbody->GetPosition().x, 0), 0.0f);

                if(rigidbody->GetPosition().y > _windowHeight- 50)
                    rigidbody->SetTransform(b2Vec2(rigidbody->GetPosition().x, _windowHeight - 50), 0.0f);
            }

            if(_currentOp == ARS_INSERT_EDGE)
            {
                if(_insertion)
                {
                    _opVertex  = NULL;
                    selected  = false;
                    state     = ARS_BTN_NORMAL;
                    _insertion = false;
                    _out       = 0x0;
                }
            }

            if(_currentOp == ARS_REMOVE_EDGE)
            {
                if(_removed)
                {
                    _opVertex  = NULL;
                    selected  = false;
                    state     = ARS_BTN_NORMAL;
                    _removed   = false;
                    _out       = 0x0;
                }
            }

            if(_currentOp == ARS_LABEL)
            {
                int c = (int)_pressedC;

                if(glfwGetKey((GLFWwindow*)_win, GLFW_KEY_ENTER))
                {
                    state     = ARS_BTN_NORMAL;
                    _labelB   = false;
                }
                else if((_charPressed        [GLFW_KEY_BACKSPACE] == GLFW_PRESS   ||
                         _charPressed        [GLFW_KEY_BACKSPACE] == GLFW_REPEAT) &&
                        _charPressedPrevious [GLFW_KEY_BACKSPACE] != GLFW_PRESS)
                {
                    if(labelV.size() > 0)
                    {
                        labelV = labelV.substr(0, labelV.size()-1);
                    }
                }
                else if(_charPressed[c] == GLFW_PRESS && _charPressedPrevious[c] != GLFW_PRESS)
                {
                    labelV += _pressedC;
                }
            }

            if(_currentOp == ARS_LABEL_EDGE)
            {
                if(_in != NULL)
                {
                    int c = (int)_pressedC;


                    if(glfwGetKey((GLFWwindow*)_win, GLFW_KEY_ENTER) || !hasEdgeTo(_in->getId(), _drawArrow))
                    {
                        state     = ARS_BTN_NORMAL;
                        _out      = NULL;
                        _in       = NULL;
                        _opVertex = NULL;
                        selected  = false;
                    }
                    else
                    {
                        int weight     = _out->getEdgeWeight(_in->getId(), _drawArrow);
                        string weightS = to_string(weight);

                        if((_charPressed        [GLFW_KEY_BACKSPACE] == GLFW_PRESS   ||
                                _charPressed        [GLFW_KEY_BACKSPACE] == GLFW_REPEAT) &&
                                _charPressedPrevious [GLFW_KEY_BACKSPACE] != GLFW_PRESS)
                        {
                            if(weightS.size() > 0)
                            {
                                weightS = weightS.substr(0, weightS.size()-1);
                            }
                        }
                        else if(_charPressed[c] == GLFW_PRESS && _charPressedPrevious[c] != GLFW_PRESS)
                        {
                            if(c >= 48 && c <= 57 && weightS.size() < ARS_MAX_WEIGHT_SIZE)
                                weightS += _pressedC;
                        }

                        weight = atoi(weightS.c_str());

                        //if(weight > ARS_MAX_EDGE_WEIGHT)
                        //    weight = ARS_MAX_EDGE_WEIGHT;

                        /** Configure edge weight **/
                        _out->setEdgeWeight(_in->getId(), weight, _drawArrow);
                    }//END ELSE
                }//END IF _IN != NULL
            }//END IF

            break;
        case ARS_BTN_OVER:
            if(_mouseButtonState        [GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS &&
                    _mousePreviousButtonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_PRESS)
            {
                if(_currentOp == ARS_MOVE_VERTEX)
                {
                    if(_opVertex == 0x0)
                    {
                        state    = ARS_BTN_PRESSED;
                        _opVertex = this;
                        selected = true;

                        /** Create mouse joint here **/
                       /* b2MouseJointDef jointDef;
                        jointDef.bodyA = _mouseClickObject->rigidbody;
                        jointDef.bodyB = rigidbody;
                        jointDef.target.Set(_mousePosition2D_world.x, _mousePosition2D_world.y);
                        jointDef.collideConnected = true;
                        jointDef.maxForce = 1000000000000000.0 * rigidbody->GetMass();
                        jointDef.dampingRatio = 0;
                        _mouseJointE = (b2MouseJoint*)_world->CreateJoint(&jointDef);*/

                        if(onClick != 0x0)
                        {
                            onClick();
                        }

                    }
                }

                if(_currentOp == ARS_INSERT_EDGE)
                {
                    if(_out == 0x0)
                    {
                        if(_out != this)
                        {
                            _opVertex = this;
                            selected = true;
                            _out      = this;
                            state    = ARS_BTN_PRESSED;
                        }
                    }
                    else
                    {
                        _out->insertEdge(ID, 0, _drawArrow);
                        _insertion     = true;
                    }
                }

                if(_currentOp == ARS_REMOVE_EDGE)
                {
                    if(_out == 0x0)
                    {
                        if(_out != this)
                        {
                            _opVertex = this;
                            selected = true;
                            _out      = this;
                            state    = ARS_BTN_PRESSED;
                        }
                    }
                    else
                    {
                        _out->removeEdge(ID, _drawArrow);

                        if(!_drawArrow)
                        {
                            removeEdge(_out->ID, _drawArrow);
                        }

                        _removed = true;
                    }
                }

                if(_currentOp == ARS_REMOVE_VERTEX)
                {
                    removeVertex();
                }

                if(_currentOp == ARS_LABEL)
                {
                    if(!_labelB)
                    {
                        _labelB       = true;
                        //labelV    = "";
                        state       = ARS_BTN_PRESSED;
                    }
                }

                if(_currentOp == ARS_LABEL_EDGE)
                {
                    if(_out == NULL)
                    {
                        //_labelB     = true;
                        selected    = true;
                        _opVertex   = this;
                        _out        = this;
                        state       = ARS_BTN_PRESSED;
                    }
                    else if(_in == NULL)
                    {
                        _in         = this;
                        state       = ARS_BTN_NORMAL;
                    }
                }

                if(_currentOp == ARS_MARK_VERTEX)
                {
                    mark();
                }
            }
            break;
        }

        /** Cancel current operation **/
        if(_mouseButtonState[GLFW_MOUSE_BUTTON_RIGHT]         == GLFW_PRESS &&
                _mousePreviousButtonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_PRESS)
        {
            _in          = NULL;
            _out         = NULL;
            _opVertex    = NULL;
            _insertion   = false;
            _removed     = false;
            _labelB      = false;
            selected     = false;
            state        = ARS_BTN_NORMAL;
        }

    }
}
//----------------------------------------------------------------------------------------------
void ArsVertex::mark()
{
    {
        marked = !marked;

        if(!marked)
            _marks--;
        else
            _marks++;
    }
}
//----------------------------------------------------------------------------------------------
void ArsVertex::removeVertex()
{
    /** Remove da cena **/
    _sceneVertex[ID] = NULL;

    /** Coloca como disponivel **/
    _availableVertex.push(this);

    /** Remove todas as referencias existentes **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        if(_sceneVertex[i]!=NULL)
        {
            _sceneVertex[i]->edges        [ID] = -1;
            _sceneVertex[i]->edgesDirected[ID] = -1;
        }
    }

    /** Limpa minha lista de arestas **/
    for(int i = 0; i < ARS_TOTAL_VERTEX; i++)
    {
        edges        [i] = -1;
        edgesDirected[i] = -1;
    }

    /** Desabilitar corpo rigido **/
    rigidbody->SetActive(false);

    /** Limpa label **/
    labelV.clear();
}
//----------------------------------------------------------------------------------------------
void ArsVertex::removeEdge(int in, bool direct)
{
    if(direct)
    {
        edgesDirected[in] = -1;

        if(_sceneVertex[in]->getEdgeWeight(ID,_drawArrow)<0)
        {
            edges        [in] = -1;
            _sceneVertex [in]->setNonEdgeWeight(ID, -1);
        }
        else
        {
            int weight = _sceneVertex[in]->getEdgeWeight(ID, direct);
            edges        [in] = weight;
            _sceneVertex [in]->setNonEdgeWeight(ID, weight);
        }
    }
    else
    {
        _sceneVertex [in]->setNonEdgeWeight(ID, -1);
        edges        [in] = -1;
        edgesDirected[in] = -1;
        _sceneVertex [in]->setDirectedEdgeWeight(ID, -1);
    }
}
//----------------------------------------------------------------------------------------------
void ArsVertex::setIcon(ArsImage* icon, float iconScale)
{
    this->icon      = icon;
    this->iconScale = iconScale;
}
//----------------------------------------------------------------------------------------------
void ArsVertex::onCollisionEnter(_2DObject* collider)
{
    if(collider->tag == MOUSE)
    {
        if(_currentOp == ARS_MOVE_VERTEX || _currentOp == ARS_INSERT_EDGE || _currentOp == ARS_REMOVE_EDGE)
        {
            if(selected == false)
            {
                state = ARS_BTN_OVER;
            }
        }
        else if(_currentOp == ARS_LABEL)
        {
            if(_labelB == false)
            {
                state = ARS_BTN_OVER;
            }
        }
        else if(_currentOp == ARS_LABEL_EDGE)
        {
            if(_out == NULL || _in == NULL)
            {
                state = ARS_BTN_OVER;
            }
        }
        else
        {
            state = ARS_BTN_OVER;
        }
    }
}
//----------------------------------------------------------------------------------------------
void ArsVertex::onCollisionEnd(_2DObject* collider)
{
    if(collider->tag == MOUSE)
    {
        if(_currentOp == ARS_MOVE_VERTEX || _currentOp == ARS_INSERT_EDGE || _currentOp == ARS_REMOVE_EDGE)
        {
            if(selected == false)
            {
                state = ARS_BTN_NORMAL;
            }
        }
        else if(_currentOp == ARS_LABEL)
        {
            if(_labelB == false)
            {
                state = ARS_BTN_NORMAL;
            }
        }
        else if(_currentOp == ARS_LABEL_EDGE)
        {
            if(_out == NULL)
            {
                state = ARS_BTN_NORMAL;
            }
        }
        else
        {
            state = ARS_BTN_NORMAL;
        }
    }
}
