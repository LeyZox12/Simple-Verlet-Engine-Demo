#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H
#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <iostream>
using namespace sf;
class physicsEngine
{
public:
    physicsEngine();
    void applyConstraints();
    bool rectCollision(RectangleShape r1, RectangleShape r2);
    bool buttonCollision(Vector2i mousePos, RectangleShape button);
    void createBall(Vector2f position, bool shouldBeStatic, bool shouldCollide);
    void addConstraint(int firstElement, int secondElement, std::string constraintType);
    void addConstraint(int firstElement, int secondElement, std::string constraintType, float maxDist);
    std::string toString(int n);
    float getDist(Vector2f pos1, Vector2f pos2);
    void drawLine(Vector2f pos1, Vector2f pos2, RenderWindow *window);
    Vector2f normalize(Vector2f vec);
    class ball
    {
    public:
        float radius;
        bool isStatic;
        bool isEngine;
        bool shouldCollide;
        CircleShape sprite;
        Vector2f position;
        Vector2f acc;
        Vector2f position_old;
        Color color;
        Vector2f anchorPoint;
        void collide();
        int anchorPointsIndex[100];
        int anchorCount;
        std::string constraintMode[100];
        int maxDist[100];
        void applyConstraints(ball balls, int subSteps);
        float friction;
        ball(){

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
            isEngine=false;

            position_old = sprite.getPosition();
            sprite.setPosition(newPos.x, newPos.y);
            }

        }


    };
int subSteps=2;
int ballAmount=0;
ball balls[20000];
protected:

private:
};

#endif
