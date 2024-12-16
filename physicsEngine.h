#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H
#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <vector>
#include <iostream>
using namespace sf;
class physicsEngine
{
public:
    physicsEngine();
    void applyConstraints(int maxThreads);
    void applyConstraintsThread(int startingPoint, int endPoint);
    void iterate();
    bool rectCollision(RectangleShape r1, RectangleShape r2);
    bool buttonCollision(Vector2i mousePos, RectangleShape button);
    void createBall(Vector2f position, bool shouldBeStatic, bool shouldCollide);
    void addConstraint(int firstElement, int secondElement, std::string constraintType);
    void addConstraint(int firstElement, int secondElement, std::string constraintType, float maxDist);
    void removeBall(int ballIndex);
    void removeConstraint(int firstElement, int secondElement);
    std::string toString(int n);
    float getDist(Vector2f pos1, Vector2f pos2);
    void drawLine(Vector2f pos1, Vector2f pos2, RenderWindow *window);
    Vector2f normalize(Vector2f vec);
    bool ballRectCollision(CircleShape ball, RectangleShape rect);
    void generateExplosion(Vector2f position, float rad, float pow);
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
        std::string constraintMode[1000];
        int maxDist[1000];
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
            position_old = sprite.getPosition();
            sprite.setPosition(newPos.x, newPos.y);
            }

        }
        float roundClose(float v)
        {
            return std::round(v*10)/10;
        }
        std::string tostr(float v)
        {
            std::string str;
            std::stringstream ss;
            ss<<roundClose(v);
            ss>>str;
            return str;
        }

        void showStats(RenderWindow& window,Font font)
        {
            RectangleShape rect(Vector2f(300,200));
            rect.setFillColor(Color(0,0,0,100));
            Text stat;
            std::string staticStr = isStatic ? "True" : "False";
            Vector2f vel = sprite.getPosition() - position_old;
            stat.setFont(font);
            stat.setFillColor(Color::White);
            stat.setCharacterSize(16);
            rect.setPosition(Vector2f(sprite.getPosition() + Vector2f(20+sprite.getRadius(),-rect.getSize().y)));
            window.draw(rect);
            stat.setPosition(rect.getPosition() + Vector2f(10,10));
            stat.setString("index:"+ tostr(index) + "\nposX:" + tostr(sprite.getPosition().x) + "  posY:" + tostr(sprite.getPosition().y) +
                           "\nConstraint count:" + tostr(anchorCount) + "\nStatic:" + staticStr + "\nvelX:" + tostr(vel.x) + "  velY:" + tostr(vel.y));
            window.draw(stat);

        }
    };
int subSteps=2;
int ballAmount;
int rotationSpeed;
float constraintStrength = 0.1;
float springStrength = 0.01;
ball balls[20000];
std::vector<RectangleShape> rects;
protected:

private:
};

#endif
