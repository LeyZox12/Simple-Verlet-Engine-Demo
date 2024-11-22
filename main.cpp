#include <iostream>
#include <SFML/Graphics.hpp>
#include "../../class/physicsEngine.h"
#include <time.h>
using namespace sf;
using namespace std;
typedef Vector2f vec2;
physicsEngine gm;
RenderWindow window(VideoMode(960,540), "PlayGround");
Event e;
Font font;
Texture texture;
bool UIselection(RectangleShape rect);
int mode=0;
int rad = 15;
void addConstraint();
bool isHolding;
bool isPaused;
bool shouldBeStatic= false;
int getSelectedBall() ;
void drag();
int firstElement = -1;
int secondElement= -1;
int targetIndex = -1;
float clamp(float minv, float maxv, float v);
int currentConstraintMode = 0;
const string constraintMode[3] = {"Rigid","Rope", "Spring" };
void initialize();
Text constraintText;
RectangleShape cur(vec2(5,5));
RectangleShape ui[5] = {RectangleShape(vec2(250,540)),RectangleShape(vec2(100,540)), RectangleShape(vec2(960,100)), RectangleShape(vec2(960,100)) };
RectangleShape buttons[12] = {RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),
                              RectangleShape(vec2(45,45)),RectangleShape(vec2(45,45)),RectangleShape(vec2(45,45)),RectangleShape(vec2(45,45)),RectangleShape(vec2(45,45)),
                              RectangleShape(vec2(45,45)),RectangleShape(vec2(45,45))
                             };
/*
Buttons:
0->addBall
1->removeConstraint
2->addConstraint
3->moveBallOrRect
4->shouldBeStatic
*/

void initialize(){
    vec2 origin = vec2(window.getSize().x/2, window.getSize().y/2);
    gm.createBall(origin, true, true);
    gm.balls[0].sprite.setPosition(origin);
 for(int i =1; i<50; i++){
    gm.createBall(vec2(origin.x+i*1, origin.y), false, true);
    gm.balls[i].sprite.setPosition(vec2(origin.x+i*5, origin.y));

    gm.balls[i].sprite.setRadius(5);
    gm.balls[i].sprite.setOrigin(vec2(7.5,7.5));
    gm.balls[i].sprite.setFillColor(Color::Black);
    gm.addConstraint(i,i-1,"Rigid");

 }
}

int main()
{

    initialize();
    if(!font.loadFromFile("res/font.ttf"))
        cout <<"Error, could not load font.ttf\n";
    if(!texture.loadFromFile("res/spriteSheet.png"))
        cout<<"Error, could not load spriteSheet.png\n";
    constraintText.setPosition(0,0);
    constraintText.setFont(font);

    gm.subSteps = 10;
    cur.setFillColor(Color::Green);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
    for(int i =0; i< 4; i++)
        ui[i].setFillColor(Color::Black);
    for(int i =0; i<5; i++)
        buttons[5+i].setTexture(&texture);
    buttons[5].setTextureRect({0,0,16,16});
    buttons[6].setTextureRect({16,0,16,16});
    buttons[7].setTextureRect({32,0,16,16});
    buttons[8].setTextureRect({0,16,16,16});
    buttons[9].setTextureRect({shouldBeStatic? 16:32,16,16,16});
    for(int i = 0; i<2; i++)
    {
        buttons[0+i*5].setPosition(100+7.5*i,140+7.5*i);
        buttons[1+i*5].setPosition(170+7.5*i,140+7.5*i);
        buttons[2+i*5].setPosition(100+7.5*i,210+7.5*i);
        buttons[3+i*5].setPosition(170+7.5*i,210+7.5*i);
        buttons[4+i*5].setPosition(100+7.5*i,280+7.5*i);
    }//Remplis les espaces vide par des espaces
    ui[1].setPosition(860,0);
    ui[2].setPosition(0,0);
    ui[3].setPosition(0,440);


    while (window.isOpen())
    {
        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();
            if(e.type == Event::KeyReleased)
            {
                if(e.key.code == Keyboard::Left)
                    currentConstraintMode = currentConstraintMode == 0?2: currentConstraintMode-1;
                if(e.key.code == Keyboard::Right)
                    currentConstraintMode = currentConstraintMode == 2?0: currentConstraintMode+1;
                constraintText.setString("Constraint mode: " + constraintMode[currentConstraintMode]);
            }
            if(e.type == Event::MouseWheelScrolled)
            {
                int scroll = e.mouseWheelScroll.delta;
                if(scroll<0 && rad>5)
                    rad += scroll;
                else if(scroll>0)
                    rad+=scroll;
            }
            if(e.type == Event::MouseButtonPressed)
            {
                if(Mouse::getPosition(window).x>250
                        && Mouse::getPosition(window).x < window.getSize().x - 100
                        && Mouse::getPosition(window).y > 100
                        && Mouse::getPosition(window).y < window.getSize().y - 100)
                {
                    isHolding = true;
                    switch (mode)
                    {
                    case(0):
                        gm.balls[gm.ballAmount].friction = 0.5;
                        gm.createBall(cur.getPosition(), shouldBeStatic, true);
                        gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                        gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
                        gm.balls[gm.ballAmount-1].sprite.setOrigin(vec2(rad,rad));
                        break;
                    case(1):
                        break;
                    case(2):
                        addConstraint();
                        break;
                    case(4):
                        break;
                    }

                }
                for(int i =0; i<4; i++)
                {
                    if(UIselection(buttons[i]))
                        mode = i;
                }
                if(UIselection(buttons[4]))
                {
                    shouldBeStatic = !shouldBeStatic;
                    buttons[9].setTextureRect({shouldBeStatic? 16:32,16,16,16});
                }
            }
            if(e.type == Event::MouseButtonReleased)
            {
                targetIndex = -1;
                isHolding= false;
            }
            if(e.type == Event::KeyPressed && e.key.code == Keyboard::Space)
                isPaused = !isPaused;

        }
        CircleShape preview(rad);
        if(mode==0)
        {
            preview.setPosition(Mouse::getPosition(window).x,Mouse::getPosition(window).y);
            preview.setOrigin(rad,rad);
            preview.setFillColor(Color(50,50,50,50));
        }
        if(mode == 3&&isHolding)
            drag();
        cur.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
        if(!isPaused)
            gm.applyConstraints();

        window.clear(Color::White);
//DRAW HERE
        for(RectangleShape u: ui)
            window.draw(u);
        for(RectangleShape b:buttons)
            window.draw(b);

        window.draw(constraintText);
        for(int i =0; i<gm.ballAmount; i++)
        {

            if(!isPaused)
            {
                gm.balls[i].acc.y +=9.8;
                gm.balls[i].upDatePos((float)1/60);
            }
            if(gm.balls[i].sprite.getPosition().y <= 100+gm.balls[i].sprite.getRadius())
            {
                gm.balls[i].sprite.setPosition(gm.balls[i].sprite.getPosition().x,100+gm.balls[i].sprite.getRadius());
                gm.balls[i].updateFriction();
            }
            else if(gm.balls[i].sprite.getPosition().y >= window.getSize().y-100-gm.balls[i].sprite.getRadius())
            {
                gm.balls[i].sprite.setPosition(gm.balls[i].sprite.getPosition().x,window.getSize().y-100-gm.balls[i].sprite.getRadius());
                gm.balls[i].updateFriction();

            }
            else{
                gm.balls[i].acc.x = 0;
            }


            if(gm.balls[i].sprite.getPosition().x <= 250 + gm.balls[i].sprite.getRadius())
                gm.balls[i].sprite.setPosition(250 + gm.balls[i].sprite.getRadius(), gm.balls[i].sprite.getPosition().y);
            if(gm.balls[i].sprite.getPosition().x >= 860 - gm.balls[i].sprite.getRadius())
                gm.balls[i].sprite.setPosition(860-gm.balls[i].sprite.getRadius(),gm.balls[i].sprite.getPosition().y);
            VertexArray line(LinesStrip, 2);
            for(int c =0; c<gm.balls[i].anchorCount; c++)
            {
                line[0].color = Color::Black;
                line[1].color = Color::Black;
                line[0].position = gm.balls[i].sprite.getPosition();
                line[1].position = gm.balls[gm.balls[i].anchorPointsIndex[c]].sprite.getPosition();
                window.draw(line);
            }
            window.draw(gm.balls[i].sprite);
            gm.balls[i].acc.y *=0.9;
        }

        window.draw(cur);
        if(mode==0)
            window.draw(preview);
        window.display();

    }

    return 0;
}
void addConstraint()
{
    if(firstElement >=0)  // if firstElement already assigned
    {
        if(getSelectedBall() ==0)
            return;
        else if(getSelectedBall()-1 != firstElement)
            secondElement =  getSelectedBall()-1;
        else
            return;
        gm.balls[secondElement].anchorPointsIndex[gm.balls[secondElement].anchorCount] = firstElement;
        gm.balls[firstElement].anchorPointsIndex[gm.balls[firstElement].anchorCount] = secondElement;
        gm.balls[secondElement].maxDist[gm.balls[secondElement].anchorCount] = gm.getDist(gm.balls[firstElement].sprite.getPosition(),gm.balls[secondElement].sprite.getPosition() );
        gm.balls[firstElement].maxDist[gm.balls[firstElement].anchorCount] = gm.getDist(gm.balls[firstElement].sprite.getPosition(),gm.balls[secondElement].sprite.getPosition() );
        gm.balls[firstElement].constraintMode[gm.balls[firstElement].anchorCount] =constraintMode[currentConstraintMode];
        gm.balls[secondElement].constraintMode[gm.balls[secondElement].anchorCount] =constraintMode[currentConstraintMode];
        gm.balls[secondElement].anchorCount++;
        gm.balls[firstElement].anchorCount++;
        firstElement = -1;
        secondElement = -1;
    }
    else
    {

        if(getSelectedBall() ==0)
        {
            return;
        }
        else
        {
            firstElement = getSelectedBall()-1;
        }
    }
}
int getSelectedBall()  // if ball is found return i else return 0, true index = i - 1
{
    for(int i = 0; i<gm.ballAmount; i++)
    {
        if(gm.getDist(vec2(Mouse::getPosition(window).x,Mouse::getPosition(window).y), gm.balls[i].sprite.getPosition()) < gm.balls[i].sprite.getRadius()*2)
        {

            return i+1;
        }
    }
    return 0;
}
bool UIselection(RectangleShape rect)
{
    Vector2f mousePos = cur.getPosition();
    if(mousePos.x >= rect.getPosition().x
            && mousePos.x<= rect.getPosition().x+rect.getSize().x
            && mousePos.y >= rect.getPosition().y
            && mousePos.y <= rect.getPosition().y+rect.getSize().y)

        return true;
    else return false;
}
void drag()
{
    if(isHolding && targetIndex == -1) //if target is not defined
    {
        for(int i =0; i<gm.ballAmount; i++)
        {
            int dist = gm.getDist(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y), gm.balls[i].sprite.getPosition());

            if( dist < gm.balls[i].sprite.getRadius()*2)
            {
                targetIndex = i;
            }
        }
    }
    else if(isHolding && targetIndex != -1)
        //gm.balls[targetIndex].acc=vec2((Mouse::getPosition(window).x-gm.balls[targetIndex].sprite.getPosition().x)/2, (Mouse::getPosition(window).y-gm.balls[targetIndex].sprite.getPosition().y)/2);
        gm.balls[targetIndex].sprite.move((Mouse::getPosition(window).x-gm.balls[targetIndex].sprite.getPosition().x)/2, (Mouse::getPosition(window).y-gm.balls[targetIndex].sprite.getPosition().y)/2);
    else if(!isHolding && targetIndex !=-1)
    {
        gm.balls[targetIndex].acc = Vector2f(0,0);
        targetIndex = -1;
    }
}
float clamp(float minv, float maxv, float v){
    if(v<minv)
        return minv;
    else if(v>maxv)
        return maxv;
    else
        return v;
}
