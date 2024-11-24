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
int fps();
int spacing = 2;
int points = 40;
void addConstraint();
void removeConstraint();
bool isHolding;
bool isPaused;
bool shouldShow = true;
bool shouldBeStatic= false;
int getSelectedBall() ;
void drag();
vec2 getTextureRect(int x, int y);
int firstBall = -1;
int firstElement = -1;
int secondElement= -1;
int targetIndex = -1;
float clamp(float minv, float maxv, float v);
int currentConstraintMode = 0;
const string constraintMode[3] = {"Rigid","Rope", "Spring" };
void initialize();
Text paramText;
vec2 grid(int x,int y, int w);
RectangleShape cur(vec2(5,5));
RectangleShape ui[5] = {RectangleShape(vec2(250,540)),RectangleShape(vec2(100,540)), RectangleShape(vec2(960,100)), RectangleShape(vec2(960,100)) };
RectangleShape buttons[8] = {
                              RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),
                              RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60)),RectangleShape(vec2(60,60))
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
    vec2 origin = vec2(300,300);
 /*for(int i =0; i<50; i++){

    gm.balls[gm.ballAmount].sprite.setPosition(vec2(origin.x+i*5, origin.y));
    gm.balls[gm.ballAmount].sprite.setRadius(5);
    gm.balls[gm.ballAmount].sprite.setOrigin(vec2(5,5));
    gm.balls[gm.ballAmount].sprite.setFillColor(Color::Black);
    gm.createBall(vec2(origin.x+i*5, origin.y), false, true);
    if(i>0)
    gm.addConstraint(gm.ballAmount,gm.ballAmount-1,"Rigid");
 }
 /*for(int i=0; i<4; i++){
    for(int j=1; j<50; j++){
        gm.createBall(vec2(origin.x+i*1, origin.y), true, true);
        gm.balls[gm.ballAmount+i].sprite.setPosition(vec2(origin.x+i*5, origin.y));
        gm.balls[gm.ballAmount+i].sprite.setRadius(5);
        gm.balls[gm.ballAmount+i].sprite.setOrigin(vec2(5,5));
        gm.balls[gm.ballAmount+i].sprite.setFillColor(Color::Black);
        gm.addConstraint(gm.ballAmount+i,gm.ballAmount+i-1,"Rigid");
    }
 }*/
}

int main()
{

    initialize();
    if(!font.loadFromFile("res/font.ttf"))
        cout <<"Error, could not load font.ttf\n";
    if(!texture.loadFromFile("res/spriteSheet.png"))
        cout<<"Error, could not load spriteSheet.png\n";
    paramText.setPosition(0,0);
    paramText.setFont(font);

    gm.subSteps = 2;
    cur.setFillColor(Color::Green);
    window.setVerticalSyncEnabled(true);

    for(int i =0; i< 4; i++)
        ui[i].setFillColor(Color::Black);
    for(auto& b:buttons)
        b.setFillColor(Color::White);


        for(int i =0; i<8; i++){
            buttons[i].setTextureRect({(i%3)*16,round((i/3))*16,16,16});
            buttons[i].setPosition(grid(i,1,2));
        }

        for(auto& b:buttons)
        {
           window.draw(b);
           b.setTexture(&texture);
        }


        for(auto& b: buttons)
        {
            b.setSize(vec2(b.getSize().x-15, b.getSize().y-15));
        }

    //Remplis les espaces vide par des espaces
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
                switch(mode){
                    case(2):
                        currentConstraintMode = currentConstraintMode == 0?2: currentConstraintMode-1;
                        break;
                    case(5):
                        if(spacing>1)
                            spacing-=1;
                        break;
                    case(7):
                        if(points>3)
                            points-=1;
                        break;
                }

                if(e.key.code == Keyboard::Right)
                    switch(mode){
                        case(2):
                            currentConstraintMode = currentConstraintMode == 2?0: currentConstraintMode+1;
                            break;
                        case(5):
                            spacing+=1;
                            break;
                        case(7):
                        if(points<360)
                            points+=1;
                        break;
                    }
                switch(mode){
                        case(2):
                            paramText.setString("Constraint mode: " + constraintMode[currentConstraintMode]);
                            break;
                        case(5):
                            paramText.setString("Spacing: " +gm.toString(spacing)+" radius");
                            break;
                        case(7):
                            paramText.setString("Points: " +gm.toString(points));
                            break;
                }

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
                        removeConstraint();
                        break;
                    case(2):
                        addConstraint();
                        break;
                    case(4):{
                        vec2 mousepos = vec2(Mouse::getPosition(window).x,Mouse::getPosition(window).y);
                        for(int b=0; b<gm.ballAmount;b++){
                            float dist = gm.getDist(mousepos, gm.balls[b].sprite.getPosition());
                            if(dist < gm.balls[b].sprite.getRadius())
                                gm.balls[b].isStatic = !gm.balls[b].isStatic;
                        }
                        }
                        break;
                    case(7):
                        double rad = 1/(180/3.14);

                        vec2 origin = vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
                        std::cout<<origin.x<<"\n";
                        int index = gm.ballAmount;


                        gm.createBall(origin, false, true);
                        gm.balls[gm.ballAmount-1].sprite.setPosition(origin);
                        gm.balls[gm.ballAmount-1].sprite.setRadius(5);
                        gm.balls[gm.ballAmount-1].sprite.setOrigin(5,5);
                        gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                        double ratioAngle = (double)360/points;
                        for(int i = 0; i<points; i++){

                            gm.createBall(origin, false, true);
                            gm.balls[gm.ballAmount-1].sprite.setPosition(origin.x+sin(gm.ballAmount*ratioAngle*rad) * 50 , origin.y+cos(gm.ballAmount*ratioAngle*rad) * 50 );
                            gm.balls[gm.ballAmount-1].sprite.setRadius(5);
                            gm.balls[gm.ballAmount-1].sprite.setOrigin(5,5);
                            gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                            gm.balls[gm.ballAmount-1].friction=1.0;
                            gm.addConstraint(gm.ballAmount-1,gm.ballAmount-2,"Rigid");
                            gm.addConstraint(gm.ballAmount-1,index,"Rigid");
                        }
                        gm.addConstraint(index+1, gm.ballAmount-1,"Rigid");
                        break;

                    }

                }
                int i =0;
                for(auto& b: buttons)
                {
                    if(UIselection(b))
                        mode = i;
                    i++;
                }
                if(UIselection(buttons[4]))
                {
                    shouldBeStatic = !shouldBeStatic;
                    buttons[9].setTextureRect({shouldBeStatic? 16:32,shouldBeStatic? 16:32,16,16});
                }
                if(UIselection(buttons[6]))
                    shouldShow = !shouldShow;
            }
            if(e.type == Event::MouseButtonReleased)
            {
                targetIndex = -1;
                firstBall= -1;
                isHolding= false;
            }
            if(e.type == Event::KeyPressed && e.key.code == Keyboard::Space)
                isPaused = !isPaused;

        }
        CircleShape preview(rad);
        if(mode==0 || mode == 5)
        {
            preview.setPosition(Mouse::getPosition(window).x,Mouse::getPosition(window).y);
            preview.setOrigin(rad,rad);
            preview.setFillColor(Color(50,50,50,50));
        }
        if(mode == 3&&isHolding)
            drag();
        cur.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
        if(mode == 5 && isHolding){
            static int d=0;
            if(firstBall!=-1)
                d = gm.getDist(gm.balls[firstBall].sprite.getPosition(),vec2(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
            if(d>rad*spacing || firstBall ==-1)
            {

                gm.createBall(cur.getPosition(), shouldBeStatic, true);
                if(firstBall != -1)
                    gm.addConstraint(firstBall, gm.ballAmount-1,"Rigid");
                gm.balls[gm.ballAmount-1].friction = 0.5;
                gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
                gm.balls[gm.ballAmount-1].sprite.setOrigin(vec2(rad,rad));
                firstBall = gm.ballAmount-1;

            }
        }
        if(!isPaused)
            gm.applyConstraints();

        window.clear(Color::White);
        window.setTitle("Physics Playground FPS:" + gm.toString(fps()));
//DRAW HERE
        for(RectangleShape u: ui)
            window.draw(u);
        for(RectangleShape b:buttons)
            window.draw(b);

        window.draw(paramText);
        for(int i =0; i<gm.ballAmount; i++)
        {

            if(!isPaused)
            {
                gm.balls[i].acc.y +=98.8;
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
            if(shouldShow)
                window.draw(gm.balls[i].sprite);
            gm.balls[i].acc.y *=0.9;
        }


        if(mode==0 || mode==5)
            window.draw(preview);
        int selected =getSelectedBall();
        if(selected>0){
            int selectedBallRadius =gm.balls[selected-1].sprite.getRadius();
            CircleShape selectionCircle = CircleShape(selectedBallRadius);
            selectionCircle.setFillColor(Color::Green);
            selectionCircle.setOrigin(selectedBallRadius,selectedBallRadius);
            selectionCircle.setPosition(gm.balls[selected-1].sprite.getPosition());
            window.draw(selectionCircle);
            selectionCircle.setRadius(selectedBallRadius-3);
            selectionCircle.setFillColor(Color::Black);
            selectionCircle.setOrigin(selectedBallRadius-3,selectedBallRadius-3);
            window.draw(selectionCircle);
        }
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
void removeConstraint()
{
    static int firstElement = -1; // default value
    static int secondElement = -1;
    if(firstElement >=0)  // if firstElement already assigned
    {
        if(getSelectedBall() ==0)
        {

            return;
        }
        else
        {
            secondElement = getSelectedBall()-1;

        }
        gm.removeConstraint(firstElement,secondElement);
        firstElement = -1;
        secondElement = -1;
    }
    else if(getSelectedBall()!=0)
        firstElement = getSelectedBall()-1;

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
vec2 grid(int x,int y, int w){
    vec2 origin = vec2(100,140);
    y+=round(x/2);
    return vec2(origin.x +70*(x%w), origin.y+70*y);
}
int fps(){
    static int t1 = clock();
    static int fps= 0;
    static int lastFps;

    if(t1 - clock() <=-1000){
        lastFps = fps;
        fps = 0;
        t1 = clock();

    }
    else
    {
        fps++;
    }
    return lastFps;
}
