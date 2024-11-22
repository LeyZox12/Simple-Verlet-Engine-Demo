#include "physicsEngine.h"
#include <iostream>
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

}
float physicsEngine::getDist(Vector2f pos1, Vector2f pos2)
{

    return sqrt(pow(pos2.x - pos1.x,2)+pow(pos2.y - pos1.y,2) );


}
void physicsEngine::applyConstraints()
{
    for(int s = 0; s<subSteps; s++)
    {
        for(int i = 0; i<ballAmount; i++)
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
                        Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*0.1),
                                                  balls[i].sprite.getPosition().y - dir.y *(difference*0.1) );
                        balls[i].sprite.setPosition(fixed);
                        dir = normalize(Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()-balls[i].sprite.getPosition()));
                        fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *difference*0.1,
                                                  balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *difference*0.1 );
                        balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                        }
                        else if(balls[i].isStatic)
                        {
                            dir = normalize(Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()-balls[i].sprite.getPosition()));
                            fixed = Vector2f(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - dir.x *difference*0.2,
                                                      balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - dir.y *difference*0.2 );
                            balls[balls[i].anchorPointsIndex[c]].sprite.setPosition(fixed);
                        }
                        else if(balls[balls[i].anchorPointsIndex[c]].isStatic)
                        {
                            Vector2f dir = normalize(Vector2f(balls[i].sprite.getPosition()-balls[balls[i].anchorPointsIndex[c]].sprite.getPosition()));
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*0.2),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*0.2) );
                            balls[i].sprite.setPosition(fixed);
                        }
                    }
                    if(balls[i].constraintMode[c] == "Rope" )
                    {
                        if(dist > balls[i].maxDist[c]  && dist != 0 )
                        {
                            balls[i].sprite.setPosition(balls[i].sprite.getPosition().x +(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - balls[i].sprite.getPosition().x )/(balls[i].maxDist[c]*2-dist) *0.5,
                                                        balls[i].sprite.getPosition().y + (balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - balls[i].sprite.getPosition().y )/(balls[i].maxDist[c]*2-dist) *0.5
                                                       );
                        }
                    }
                    if(balls[i].constraintMode[c] == "Spring" )
                    {

                        if(dist > balls[i].maxDist[c]  && dist != 0 )
                        {
                            balls[i].sprite.setPosition(balls[i].sprite.getPosition().x +(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - balls[i].sprite.getPosition().x )/(balls[i].maxDist[c]*2-dist) *0.1,
                                                        balls[i].sprite.getPosition().y + (balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - balls[i].sprite.getPosition().y )/(balls[i].maxDist[c]*2-dist) *0.1
                                                       );

                        }
                        else if(dist < balls[i].maxDist[c] && dist != 0)
                        {
                            balls[i].sprite.setPosition(balls[i].sprite.getPosition().x +(balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().x - balls[i].sprite.getPosition().x )/(dist*2-balls[i].maxDist[c])* -0.1,
                                                        balls[i].sprite.getPosition().y + (balls[balls[i].anchorPointsIndex[c]].sprite.getPosition().y - balls[i].sprite.getPosition().y )/(dist*2 -balls[i].maxDist[c]) * -0.1
                                                       );
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
                            Vector2f fixed = Vector2f(balls[i].sprite.getPosition().x - dir.x *(difference*0.1),
                                                      balls[i].sprite.getPosition().y - dir.y *(difference*0.1) );
                            balls[i].sprite.setPosition(fixed);
                            dir = normalize(Vector2f(balls[n].sprite.getPosition()-balls[i].sprite.getPosition()));
                            fixed = Vector2f(balls[n].sprite.getPosition().x - dir.x *difference*0.1,
                                                      balls[n].sprite.getPosition().y - dir.y *difference*0.1 );
                            balls[n].sprite.setPosition(fixed);

                        }
                        /*else if(balls[n].isStatic)
                        {
                            balls[i].sprite.setPosition(balls[i].sprite.getPosition().x+(balls[n].sprite.getPosition().x - balls[i].sprite.getPosition().x) / (30*2  - dist)*-0.5,
                                                        balls[i].sprite.getPosition().y+(balls[n].sprite.getPosition().y - balls[i].sprite.getPosition().y) / (30*2  - dist)*-0.5);

                        }
                        else if(balls[i].isStatic)
                        {
                            balls[n].sprite.setPosition(balls[n].sprite.getPosition().x+(balls[i].sprite.getPosition().x - balls[n].sprite.getPosition().x) / (dist*2  -30)*-0.5,
                                                        balls[n].sprite.getPosition().y+(balls[i].sprite.getPosition().y - balls[n].sprite.getPosition().y) / (dist*2  -30)*-0.5);

                        }*/
                    }
                }
            }
        }
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
std::string physicsEngine::toString(int n )
{
    std::stringstream stream;
    stream<<n;
    std::string strbuffer;
    stream>>strbuffer;
    return strbuffer;

}
