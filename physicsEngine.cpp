#include "physicsEngine.h"
#include <iostream>
#include <thread>
#include <algorithm>
physicsEngine::physicsEngine()
{

}
void physicsEngine::createBall(Vector2f position,bool shouldBeStatic, bool shouldCollide)
{

    balls[ballAmount].sprite.setPosition(position);
    balls[ballAmount].sprite.setRadius(15);
    balls[ballAmount].sprite.setFillColor(Color::White);
    balls[ballAmount].position_old =  balls[ballAmount].sprite.getPosition();
    balls[ballAmount].sprite.setOrigin(15,15);
    balls[ballAmount].isStatic = shouldBeStatic;
    balls[ballAmount].shouldCollide = shouldCollide;
    balls[ballAmount].index = ballAmount;

    ballAmount++;

}
void physicsEngine::addConstraint(int firstElement, int secondElement, std::string constraintType)
{
    balls[secondElement].anchorPointsIndex[balls[secondElement].anchorCount] = firstElement;
    balls[firstElement].anchorPointsIndex[balls[firstElement].anchorCount] = secondElement;
    balls[secondElement].maxDist[balls[secondElement].anchorCount] = getDist(balls[firstElement].sprite.getPosition(),balls[secondElement].sprite.getPosition() );
    balls[firstElement].maxDist[balls[firstElement].anchorCount] = getDist(balls[firstElement].sprite.getPosition(),balls[secondElement].sprite.getPosition() );
    balls[firstElement].constraintMode[balls[firstElement].anchorCount] =constraintType;
    balls[secondElement].constraintMode[balls[secondElement].anchorCount] =constraintType;
    balls[secondElement].anchorCount++;
    balls[firstElement].anchorCount++;
}
void physicsEngine::addConstraint(int firstElement, int secondElement, std::string constraintType, float maxDist)
{
    balls[secondElement].anchorPointsIndex[balls[secondElement].anchorCount] = firstElement;
    balls[firstElement].anchorPointsIndex[balls[firstElement].anchorCount] = secondElement;
    balls[secondElement].maxDist[balls[secondElement].anchorCount] = maxDist;
    balls[firstElement].maxDist[balls[firstElement].anchorCount] = maxDist;
    balls[firstElement].constraintMode[balls[firstElement].anchorCount] =constraintType;
    balls[secondElement].constraintMode[balls[secondElement].anchorCount] =constraintType;
    balls[secondElement].anchorCount++;
    balls[firstElement].anchorCount++;

}
void physicsEngine::removeConstraint(int firstElement, int secondElement){
        int firstIndex = -1;
        int secondIndex = -1;

        for(int i =0; i<balls[firstElement].anchorCount; i++)
            if(balls[firstElement].anchorPointsIndex[i] == secondElement)
            {
                firstIndex = i;
                break;
            }
        for(int o =0; o<balls[secondElement].anchorCount; o++)
            if(balls[secondElement].anchorPointsIndex[o] == firstElement)
            {
                secondIndex=o;
                break;
            }
        balls[firstElement].maxDist[firstIndex] = balls[firstElement].maxDist[balls[firstElement].anchorCount-1];
        balls[secondElement].maxDist[secondIndex] = balls[secondElement].maxDist[balls[secondElement].anchorCount-1];
        balls[firstElement].anchorPointsIndex[firstIndex] = balls[firstElement].anchorPointsIndex[balls[firstElement].anchorCount-1];
        balls[secondElement].anchorPointsIndex[secondIndex] = balls[secondElement].anchorPointsIndex[balls[secondElement].anchorCount-1];

        balls[secondElement].anchorCount--;
        balls[firstElement].anchorCount--;
}
float physicsEngine::getDist(Vector2f pos1, Vector2f pos2)
{

    return sqrt(pow(pos2.x - pos1.x,2)+pow(pos2.y - pos1.y,2) );


}
void physicsEngine::applyConstraints(int maxThreads)
{
    std::vector<std::thread> threads;
    for(int s = 0; s<subSteps; s++)
    {
        int part = ceil((float)ballAmount/maxThreads);
        for(int t=0; t<maxThreads; t++){
            threads.emplace_back([this, t, part]() {
                applyConstraintsThread(t * part, (t + 1) * part);
            });
        }
        for(auto& t:threads)
            t.join();
        threads.clear();
    }
}
void physicsEngine::removeBall(int ballIndex)
{

        int aCount = balls[ballIndex].anchorCount;
        for(int i = 0; i < aCount; i++)
        {
            int index = balls[ballIndex].anchorPointsIndex[i];
            removeConstraint(index,ballIndex);
        }
    if(ballIndex < ballAmount - 1)
    {
        aCount = balls[ballAmount-1].anchorCount;
        balls[ballIndex] = balls[ballAmount-1];
        for(int i = 0; i < aCount; i++)
        {
            removeConstraint(ballAmount - 1,balls[ballAmount - 1].anchorPointsIndex[i]);
        }
        for(int i = 0; i < aCount; i++)
        {


            int otherIndex = balls[ballAmount-1].anchorPointsIndex[i];
            for(int j = 0; j < balls[otherIndex].anchorCount; j++)
                if(balls[otherIndex].anchorPointsIndex[j] == ballAmount - 1)
                    balls[otherIndex].anchorPointsIndex[j] = ballIndex;
        }
    }
    ballAmount--;
}
void physicsEngine::applyConstraintsThread(int startingPoint,int endPoint)
{

        for(int i = startingPoint; i<endPoint; i++)
        {
            if(!balls[i].isStatic)
            {
                for(int c = 0; c<balls[i].anchorCount; c++)
                {
                    float dist = getDist(balls[i].sprite.getPosition(), balls[balls[i].anchorPointsIndex[c]].sprite.getPosition());
                    float difference = dist - balls[i].maxDist[c];

                    if(balls[i].constraintMode[c] == "Rigid" )
                    {
                        Vector2f dir;
                        Vector2f fixed;
                        if(!balls[i].isStatic && !balls[balls[i].anchorPointsIndex[c]].isStatic)
                        {

                        Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                        Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*constraintStrength),
                                                  balls[i].sprite.getPosition().y - dir.y *(difference*constraintStrength) );
                        balls[i].sprite.setPosition(fixed);
                        dir = Vector2f(dir.x*-1,dir.y*-1);
                        fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *difference*constraintStrength,
                                         balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *difference*constraintStrength );
                        balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                        }
                        else if(balls[i].isStatic)
                        {
                            dir = normalize(Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()-balls[i].sprite.getPosition()));
                            fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *difference*constraintStrength*2,
                                                      balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *difference*constraintStrength*2 );
                            balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                        }
                        else if(balls[balls[i].anchorPointsIndex[c]].isStatic)
                        {
                            Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*constraintStrength*2),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*constraintStrength*2) );
                            balls[i].sprite.setPosition(fixed);
                        }
                    }
                    else if(balls[i].constraintMode[c] == "Rope" )
                    {
                        if(dist>balls[i].maxDist[c])
                        {
                            Vector2f dir;
                            Vector2f fixed;
                            if(!balls[i].isStatic && !balls[balls[i].anchorPointsIndex[c]].isStatic)
                            {

                            Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*constraintStrength),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*constraintStrength) );
                            balls[i].sprite.setPosition(fixed);
                            dir = Vector2f(dir.x*-1,dir.y*-1);
                            fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *difference*constraintStrength,
                                             balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *difference*constraintStrength );
                            balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                            }
                            else if(balls[i].isStatic)
                            {
                                dir = normalize(Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()-balls[i].sprite.getPosition()));
                                fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *difference*constraintStrength*2,
                                                          balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *difference*constraintStrength*2 );
                                balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                            }
                            else if(balls[balls[i].anchorPointsIndex[c]].isStatic)
                            {
                                Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                                Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*constraintStrength*2),
                                                          balls[i].sprite.getPosition().y - dir.y *(difference*constraintStrength*2) );
                                balls[i].sprite.setPosition(fixed);
                            }
                        }
                    }
                    else if(balls[i].constraintMode[c] == "Spring" )
                    {
                        Vector2f dir;
                        Vector2f fixed;
                        if(!balls[i].isStatic && !balls[balls[i].anchorPointsIndex[c]].isStatic)
                        {

                        Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                        Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*springStrength),
                                                  balls[i].sprite.getPosition().y - dir.y *(difference*springStrength) );
                        balls[i].sprite.setPosition(fixed);
                        dir = Vector2f(dir.x*-1,dir.y*-1);
                        fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *(difference*springStrength),
                                         balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *(difference*springStrength));
                        balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                        }
                        else if(balls[i].isStatic)
                        {
                            dir = normalize(Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()-balls[i].sprite.getPosition()));
                            fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *(difference*(springStrength*2)),
-                                                      balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *(difference*(springStrength*2)));
                            balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                        }
                        else if(balls[balls[i].anchorPointsIndex[c]].isStatic)
                        {
                            Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*(springStrength*2)),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*(springStrength*2)) );
                            balls[i].sprite.setPosition(fixed);
                        }
                    }
                }
            }
            for(int n = 0; n<ballAmount; n++)
            {
                bool shouldCollide = true;
                for(auto& a: balls[i].anchorPointsIndex)
                    if(a==n)
                        shouldCollide=false;
                if(balls[i].shouldCollide && balls[n].shouldCollide && shouldCollide && n!=i)
                {
                    Vector2f dir;
                    Vector2f fixed;
                    float dist = (float)getDist(balls[i].sprite.getPosition(), balls[n].sprite.getPosition());
                    float difference = dist - (balls[i].sprite.getRadius()+balls[n].sprite.getRadius());

                    if(dist <=balls[i].sprite.getRadius()+balls[n].sprite.getRadius())
                    {

                        if(!balls[n].isStatic && !balls[i].isStatic)
                        {
                            Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[n].sprite.getPosition()));
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*constraintStrength),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*constraintStrength) );
                            balls[i].sprite.setPosition(fixed);
                            dir = Vector2f(dir.x*-1,dir.y*-1);
                            fixed = Vector2f(balls[n].sprite.getPosition().x - dir.x *difference*constraintStrength,
                                                      balls[n].sprite.getPosition().y - dir.y *difference*constraintStrength );
                            balls[n].sprite.setPosition(fixed);

                        }
                        else if(balls[n].isStatic)
                        {
                            Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[n].sprite.getPosition()));
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*constraintStrength*2),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*constraintStrength*2) );
                            balls[i].sprite.setPosition(fixed);
                        }
                        else if(balls[i].isStatic)
                        {
                            dir = normalize(Vector2f(balls[n].sprite.getPosition()-balls[i].sprite.getPosition()));
                            fixed = Vector2f(balls[n].sprite.getPosition().x - dir.x *difference*constraintStrength*2,
                                                      balls[n].sprite.getPosition().y - dir.y *difference*constraintStrength*2 );
                            balls[n].sprite.setPosition(fixed);

                        }
                    }
                }
            }
        }


}
void physicsEngine::generateExplosion(Vector2f position,float rad, float pow)
{
    for(int i = 0; i<ballAmount; i++)
    {
        float dist = getDist(balls[i].sprite.getPosition(),position);
        Vector2f dir = balls[i].sprite.getPosition() - position;
        Vector2f vel = Vector2f(dir.x * rad-dist,dir.y*rad-dist);
        if(dist<rad+balls[i].sprite.getRadius())
            balls[i].acc+=vel*(pow*10);
    }
}

bool physicsEngine::rectCollision(RectangleShape r1, RectangleShape r2)
{
    if( r1.getPosition().x + r1.getSize().x >= r2.getPosition().x &&
            r1.getPosition().x <= r2.getPosition().x + r2.getSize().x &&
            r1.getPosition().y + r1.getSize().y >= r2.getPosition().y &&
            r1.getPosition().y <= r2.getPosition().y + r2.getSize().y)
        return true;
    else return false;
}
bool physicsEngine::buttonCollision(Vector2i mousePos, RectangleShape button)
{
    return(mousePos.x > button.getPosition().x && mousePos.x < button.getPosition().x + button.getSize().x&&
           mousePos.y > button.getPosition().y && mousePos.y < button.getPosition().y + button.getSize().y);
}
void physicsEngine::drawLine(Vector2f pos1, Vector2f pos2,RenderWindow *window)
{
    VertexArray line(LineStrip, 2);
    line[0].position = pos1;
    line[1].position = pos2;
    window->draw(line);
}
Vector2f physicsEngine::normalize(Vector2f vec)
{
    double dist = sqrt(pow(vec.x,2) + pow(vec.y,2));
    return Vector2f(vec.x / dist, vec.y / dist);

}
bool physicsEngine::ballRectCollision(CircleShape ball, RectangleShape rect)
{

    //choose correct side;
    float x = ball.getPosition().x;
    float y = ball.getPosition().y;
    if(x<rect.getPosition().x)
    {
        x = rect.getPosition().x;

    }
    else if(x>rect.getPosition().x + rect.getSize().x)
    x= rect.getPosition().x + rect.getSize().x;
    if(y<rect.getPosition().y)
    {
        y = rect.getPosition().y;
    }
    else if(y>rect.getPosition().y+rect.getSize().y)
        y = rect.getPosition().y + rect.getSize().y;

    Vector2f diff = Vector2f(ball.getPosition().x - x, ball.getPosition().y -y);
    float dist = sqrt(diff.x*diff.x+ diff.y*diff.y);
    if(dist<ball.getRadius())
            return true;
    return false;
}
std::string physicsEngine::toString(int n )
{
    std::stringstream stream;
    stream<<n;
    std::string strbuffer;
    stream>>strbuffer;
    return strbuffer;

}
