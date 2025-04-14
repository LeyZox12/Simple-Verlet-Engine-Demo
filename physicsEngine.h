#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H
#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <array>
#include <fstream>
#include "UIutils.h"
#include <functional>


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
        unsigned int anchorPointsIndex[1000];
        unsigned int anchorCount;
        unsigned int index;
        unsigned int maxDist[1000];
        float rotationSpeed;
        string constraintMode[1000];
        vector<string> uibuttons;
        vector<sf::RectangleShape> uibuttonsRect;
        float friction;
        ball(){
            uibuttons.push_back("freeze");
            uibuttons.push_back("cut all constraints");
            uibuttons.push_back("follow");
            for(unsigned int i = 0; i < uibuttons.size(); i++)
            {
                RectangleShape rect;
                uibuttonsRect.push_back(rect);
            }
        }
        ball(Vector2f position, Vector2f anchorPoint)
        {
            this->sprite = CircleShape(radius);
            this->sprite.setPosition(position);
            this->sprite.setOrigin(Vector2f(radius, radius));
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
            sprite.setPosition(Vector2f(newPos.x, newPos.y));
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
        void UIEventManager(Event e, Vector2f mousePos)
        {

        }
        void showStats(RenderWindow& window, Vector2f mousePos,Font font)
        {
            RectangleShape rect(Vector2f(300,200));
            rect.setFillColor(Color(0,0,0,100));
            string staticStr = isStatic ? "True" : "False";
            Vector2f vel = sprite.getPosition() - position_old;
            Text stat(font, "index:"+ tostr(index) +
                            "\nposX:" + tostr(sprite.getPosition().x) +
                            "  posY:" + tostr(sprite.getPosition().y) +
                            "\nConstraint count:" + tostr(anchorCount) +
                            "\nStatic:" + staticStr +
                            "\nvelX:" + tostr(vel.x) +
                            "  velY:" + tostr(vel.y), 16);
            stat.setFillColor(Color::White);
            rect.setPosition(Vector2f(sprite.getPosition() + Vector2f(20+sprite.getRadius(),-rect.getSize().y)));
            window.draw(rect);
            stat.setPosition(rect.getPosition() + Vector2f(10,10));
            window.draw(stat);
            for(int i = 0; i < uibuttons.size(); i++)
            {
                float ratioSizeY = rect.getSize().y / (uibuttons.size() + 2);
                float ratioSizeX = rect.getSize().x/2;
                uibuttonsRect[i].setFillColor(Color(100, 100, 100, 100));
                uibuttonsRect[i].setSize(sf::Vector2f(ratioSizeX, ratioSizeY));
                uibuttonsRect[i].setPosition(rect.getPosition() + Vector2f(ratioSizeX * (i % 2), ratioSizeY * floor(3 + i / 2)));
                stat.setPosition(uibuttonsRect[i].getPosition());
                stat.setString(uibuttons[i]);
                window.draw(uibuttonsRect[i]);
                window.draw(stat);
            }
        }
    };
    unsigned int subSteps=4;
    unsigned int ballAmount;
    unsigned int rectAmount;
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
