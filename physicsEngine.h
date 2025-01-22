#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H
#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <vector>
#include <iostream>
#include "UIutils.h"

using namespace sf;
using namespace std;

class physicsEngine
{
public:
    physicsEngine();
    void applyConstraints(int maxThreads);
    void applyConstraintsThread(int startingPoint, int endPoint);
    void iterate();
    void deleteRect(int i);
    bool rectCollision(RectangleShape r1, RectangleShape r2);
    bool buttonCollision(Vector2i mousePos, RectangleShape button);
    void createBall(Vector2f position, bool shouldBeStatic, bool shouldCollide);
    void addConstraint(int firstElement, int secondElement, string constraintType);
    void addConstraint(int firstElement, int secondElement, string constraintType, float maxDist);
    void removeBall(int ballIndex);
    void removeConstraint(int firstElement, int secondElement);
    string toString(int n);
    float getDist(Vector2f pos1, Vector2f pos2);
    void drawLine(Vector2f pos1, Vector2f pos2, RenderWindow *window);
    Vector2f normalize(Vector2f vec);
    bool ballRectCollision(CircleShape ball, RectangleShape rect);
    void generateExplosion(Vector2f position, float rad, float pow);
    void createRect(Vector2f rectPos, Vector2f rectSize);
    struct ball
    {
        float radius;
        bool isStatic;
        bool shouldShowStats;
        bool shouldCollide;
        CircleShape sprite;
        Vector2f position;
        Vector2f acc;
        Vector2f position_old;
        Color color;
        Vector2f anchorPoint;
        void collide();
        int anchorPointsIndex[1000];
        int anchorCount;
        int index;
        float rotationSpeed;
        string constraintMode[1000];
        vector<string> uibuttons;
        vector<sf::RectangleShape> uibuttonsRect;
        int maxDist[1000];
        float friction;
        ball(){
            uibuttons.push_back("freeze");
            uibuttons.push_back("cut all constraints");
            uibuttons.push_back("rotation +");
            uibuttons.push_back("rotation -");
            for(int i = 0; i < uibuttons.size(); i++)
            {
                RectangleShape rect;
                rect.setFillColor(sf::Color::Black);
                uibuttonsRect.push_back(rect);
            }
        }
        ball(Vector2f position, Vector2f anchorPoint)
        {
            this->sprite = CircleShape(radius);
            this->sprite.setPosition(position);
            this->sprite.setOrigin(radius, radius);
            this->sprite.setFillColor(Color::Green);
            this->position_old = sprite.getPosition();
            this->anchorPoint = anchorPoint;
            this->sprite.setRadius(30);
            this-> acc = Vector2f(0,0);
        }
        void updateFriction(){

            acc.x =  (position_old.x -sprite.getPosition().x) *250*friction;
        }
        void upDatePos(float dt)
        {

            if(!isStatic){
            Vector2f vel = sprite.getPosition() - position_old ;
            Vector2f newPos = Vector2f(
                                  2*sprite.getPosition().x -position_old.x +  acc.x *( dt *dt),
                                  2*sprite.getPosition().y - position_old.y +acc.y * (dt *dt)
                              );
            position_old = sprite.getPosition();
            sprite.setPosition(newPos.x, newPos.y);
            }

        }
        float roundClose(float v)
        {
            return round(v*10)/10;
        }
        string tostr(float v)
        {
            string str;
            stringstream ss;
            ss<<roundClose(v);
            ss>>str;
            return str;
        }
        bool UIselection(RectangleShape rect,sf::Vector2f mousePos)
        {
            if(mousePos.x >= rect.getPosition().x
                    && mousePos.x<= rect.getPosition().x+rect.getSize().x
                    && mousePos.y >= rect.getPosition().y
                    && mousePos.y <= rect.getPosition().y+rect.getSize().y)
                return true;
            return false;
        }
        void updateValues(sf::RenderWindow& window)
        {
            for(int i = 0; i < uibuttons.size(); i++)
            {

            }
        }
        void showStats(RenderWindow& window, Vector2f mousePos,Font font)
        {
            RectangleShape rect(Vector2f(300,200));
            rect.setFillColor(Color(0,0,0,100));
            Text stat;
            string staticStr = isStatic ? "True" : "False";
            Vector2f vel = sprite.getPosition() - position_old;
            stat.setFont(font);
            stat.setFillColor(Color::White);
            stat.setCharacterSize(16);
            rect.setPosition(Vector2f(sprite.getPosition() + Vector2f(20+sprite.getRadius(),-rect.getSize().y)));
            window.draw(rect);
            stat.setPosition(rect.getPosition() + Vector2f(10,10));
            stat.setString("index:"+ tostr(index) + "\nposX:" + tostr(sprite.getPosition().x) + "  posY:" + tostr(sprite.getPosition().y) +
                           "\nConstraint count:" + tostr(anchorCount) + "\nStatic:" + staticStr + "\nvelX:" + tostr(vel.x) + "  velY:" + tostr(vel.y) + "\nRotation Speed(constraint):" + tostr(rotationSpeed));
            window.draw(stat);
            for(int i = 0; i < uibuttons.size(); i++)
            {
                uibuttonsRect[i].setSize(sf::Vector2f(rect.getSize().x/2,16));
                uibuttonsRect[i].setPosition(rect.getPosition());
                stat.setPosition(uibuttonsRect[i].getPosition()+ sf::Vector2f(i*uibuttonsRect[i].getSize().x,
                                                                              rect.getSize().y/2+16*i));
                stat.setString(uibuttons[i]);
                window.draw(uibuttonsRect[i]);
                window.draw(stat);
            }
        }
    };
    struct customConstraintScript
    {
        class node
        {
            /*
            0:Add
            1:Sub
            2:Mult
            3:Div
            4:Pow
            5:Mod

            7:sin
            8:cos
            9:tan
            10:round
            Const
            11:x1
            12:x2
            */
            public:
                int index;
                int inputCount;
                string type;
                float value;
                vector<int> inputIndexes;
                vector<int> outputIndex;
                vector<string> outputTypes;
                Vector2f output1;
                Vector2f output2;
                Vector2f offset = Vector2f(0, 0);
                Text name;
                RectangleShape scriptVisual;
                node()
                {

                }
                void setType(string type)
                {
                    this->type = type;
                    if(type == "add" || type == "sub" || type == "mult" || type == "pow" || type == "div" || type == "mod" || type == "out")
                        inputCount = 2;
                    else if(type == "val")
                        inputCount = 0;
                }
        };
        vector<node> nodes;
        node out;
        UIutils ui;
        int centerOffset = 30;
        int grabbedIndex = -1;
        Vector2f grabbedOffset;
        Vector2f currentPos;
        vector<RectangleShape> rectangles;
        customConstraintScript()
        {
            out.setType("out");
            out.offset = Vector2f(200, 200);
            nodes.push_back(out);
            out.offset = Vector2f(200, 100);
            nodes.push_back(out);
        }
        void addNode(string type)
        {
            node newNode;
            newNode.index = nodes.size();
            newNode.setType(type);
            nodes.push_back(newNode);
        }
        void updateUI(Event e, RenderWindow& window)
        {
            ui.updateElements(e, window);
            if(e.type == Event::MouseButtonReleased)
            {
                grabbedIndex = -1;
            }
        }
        void grab(Vector2f mousePos)
        {
            for(int i = 0; i < nodes.size(); i++)
            {
                Vector2f rectPos = currentPos + nodes[i].offset;
                Vector2f rectSize = Vector2f(100, centerOffset + 10 * nodes[i].inputCount);
                cout << rectPos.x  << endl << mousePos.x<< endl;
                if(mousePos.x > rectPos.x &&
                   mousePos.x < rectPos.x + rectSize.x &&
                   mousePos.y > rectPos.y &&
                   mousePos.y < rectPos.y + rectSize.y)
                {
                    grabbedIndex = i;
                    grabbedOffset = Vector2f(mousePos - rectPos);
                    break;
                }
            }
            if(grabbedIndex > -1)
                nodes[grabbedIndex].offset = Vector2f(mousePos - currentPos - grabbedOffset);
        }
        void showScriptVisual(RenderWindow& window, Font font, Vector2f pos, Vector2f sizeR)
        {
            currentPos = pos;
            ui.displayElements(window);
            RectangleShape scriptRect;
            rectangles.clear();
            scriptRect.setPosition(pos);
            scriptRect.setSize(sizeR);
            scriptRect.setFillColor(Color(255, 255, 255, 200));
            window.draw(scriptRect);
            for(int i = 0; i < nodes.size(); i++)
            {
                RectangleShape nodeRect;
                nodeRect.setPosition(pos + nodes[i].offset);
                nodeRect.setFillColor(Color::Black);
                nodeRect.setSize(Vector2f(100, centerOffset + 10 * nodes[i].inputCount));
                window.draw(nodeRect);
                rectangles.push_back(nodeRect);
                Text label;
                label.setCharacterSize(10);
                label.setFont(font);
                label.setPosition(pos + nodes[i].offset);
                label.setString(nodes[i].type);
                label.setFillColor(Color::White);
                window.draw(label);
            }
            for(int i = 0; i < nodes.size(); i++)
            {
                CircleShape nodeCircle(5);
                for(int n = 0; n < nodes[i].inputCount; n++)
                {
                    nodeCircle.setFillColor(Color(0, 0, 0));
                    nodeCircle.setOrigin(5, 5);
                    nodeCircle.setPosition(rectangles[i].getPosition() + Vector2f(0, centerOffset + 10 * n));
                    window.draw(nodeCircle);
                    if(nodes[i].inputIndexes.size() > n)
                    {
                        VertexArray line(LineStrip, 2);
                        Vector2f rectPos = scriptRect.getPosition() + nodes[nodes[i].inputIndexes[i]].offset;
                        line[0].position = rectPos + Vector2f(rectangles[nodes[i].inputIndexes[i]].getSize().x, centerOffset);
                        line[1].position = nodeCircle.getPosition();
                        line[0].color = Color::Black;
                        line[1].color = Color::Black;
                        window.draw(line);
                    }
                }
                if(nodes[i].type != "out")
                {
                    nodeCircle.setPosition(rectangles[i].getPosition() + Vector2f(rectangles[i].getSize().x, 30));
                    window.draw(nodeCircle);
                }
            }
        }
    };
    int subSteps=4;
    int ballAmount;
    int rectAmount;
    float constraintStrength = 0.1;
    float springStrength = 0.01;
    ball balls[20000];
    RectangleShape rects[2000];
    double pi = 3.14159;
    double r = (double)pi/180;
    double d = 180/pi;
protected:

private:
};

#endif
