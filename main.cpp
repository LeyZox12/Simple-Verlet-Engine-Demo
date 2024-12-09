#include <iostream>
#include <SFML/Graphics.hpp>
#include "../../class/physicsEngine.h"
#include <time.h>
#include <vector>
using namespace sf;
using namespace std;
typedef Vector2f vec2;
physicsEngine gm;
RenderWindow window(VideoMode::getDesktopMode(), "PlayGround",Style::Fullscreen);
Event e;
Font font;
Texture texture;
bool UIselection(RectangleShape rect);
int mode=0;
int rad = 15;
int maxThreads = 8;
int fps();
int currentChain=0;
int spacing = 2;
int points = 20;
float cameraSpeed=0.5;
float explosionRad = 50.0;
float explosionPower = 50.0;
int clothHeight = 5;
void addConstraint();
void removeConstraint();
bool isHolding;
bool isMouseOnUI();
bool isPaused;
bool shouldShow = true;
bool shouldBeStatic= false;
int getSelectedBall();
bool isMovingCamera = false;
bool isExplosionReversed = false;
int buttonCount = 13;
void drag();
vec2 getTextureRect(int x, int y);
int firstBall = -1;
int firstElement = -1;
int secondElement= -1;
int targetIndex = -1;
float motorSpeed = 10.0;
float clamp(float minv, float maxv, float v);
int currentConstraintMode = 0;
const string constraintMode[3] = {"Rigid","Rope", "Spring" };
void initialize();
Text paramText;
vec2 grid(int x,int y, int w);
RectangleShape cur(vec2(5,5));
RectangleShape ui[2] = {RectangleShape(vec2(300,540)), RectangleShape(vec2(960,100)) };
vector<RectangleShape> buttons;
void start();
void applyUIAnchors();
View camera(FloatRect(0,0,960,540));
vec2 mousePos;
void start()
{
    camera.move(0,750);
    if(!font.loadFromFile("res/font.ttf"))
        cout <<"Error, could not load font.ttf\n";
    if(!texture.loadFromFile("res/spriteSheet.png"))
        cout<<"Error, could not load spriteSheet.png\n";
    paramText.setPosition(0,0);
    paramText.setFont(font);
    paramText.setColor(Color::Black);
    gm.subSteps = 4;
    gm.constraintStrength = 0.2;
    cur.setFillColor(Color::Green);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(true);
    for(int i = 0; i<buttonCount; i++)
        buttons.emplace_back(RectangleShape(vec2(60,60)));

    for(auto& u: ui)
        u.setFillColor(Color::Black);
    for(auto& b:buttons)
        b.setFillColor(Color::White);


    for(int i =0; i<buttonCount; i++)
    {
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
    ui[1].setPosition(0,1080);
    ui[2].setPosition(0,0);
}
int main()
{
    start();
    while (window.isOpen())
    {
        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();
            if(e.type == Event::MouseMoved)
            {
                if(isMovingCamera && mode == 8)
                {
                    Vector2f delta = window.mapPixelToCoords(Vector2i(Mouse::getPosition(window).x,Mouse::getPosition(window).y)) - mousePos;
                    camera.move(delta*-cameraSpeed);
                }
                mousePos = window.mapPixelToCoords(Vector2i(Mouse::getPosition(window).x,Mouse::getPosition(window).y));
            }
            if(e.type == Event::KeyReleased)
            {
                int selected = getSelectedBall();
                if(e.key.code == Keyboard::F11)
                {

                }
                if(e.key.code == Keyboard::Delete && selected>-1)
                    gm.removeBall(selected);
                if(e.key.code == Keyboard::Up)
                {
                    switch(mode)
                    {
                    case(9):
                        clothHeight++;
                        break;
                    }
                }
                if(e.key.code == Keyboard::Down)
                {
                    switch(mode)
                    {
                    case(9):
                        clothHeight--;
                        break;
                    }
                }
                if(e.key.code == Keyboard::Left)
                    switch(mode)
                    {
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
                    case(8):
                        if(cameraSpeed>0)
                            cameraSpeed-=0.1;
                        break;
                    case(9):
                        if(spacing>1)
                            spacing-=1;
                        break;
                    case(11):
                        explosionPower-=10;
                        break;
                    case(12):
                        motorSpeed-=5;
                        break;
                    }
                if(e.key.code == Keyboard::Right)
                    switch(mode)
                    {
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
                    case(8):
                        cameraSpeed+=0.1;
                        break;
                    case(9):
                        if(spacing>1)
                            spacing+=1;
                        break;
                    case(11):
                        explosionPower+=10;
                        break;
                    case(12):
                        motorSpeed+=5;
                        break;
                    }
            }
            if(e.type == Event::MouseWheelScrolled)
            {
                int scroll = e.mouseWheelScroll.delta;
                if(mode == 8)
                {
                    scroll *= 100;
                    float ratioy = camera.getSize().y/camera.getSize().x;
                    camera.setSize(camera.getSize().x-scroll,camera.getSize().y - scroll*ratioy);
                }
                else if(mode==11)
                {
                    if(scroll<0 && rad>5)
                        explosionRad += scroll;
                    else if(scroll>0)
                        explosionRad+=scroll;
                }
                else
                {
                    if(scroll<0 && rad>5)
                        rad += scroll;
                    else if(scroll>0)
                        rad+=scroll;
                }


            }

            if(e.type == Event::MouseButtonPressed)
            {

                if(e.mouseButton.button == Mouse::Left)
                {
                    isHolding = true;

                    if(!UIselection(ui[0]))
                    {
                        switch (mode)
                        {
                        case(0):
                            gm.balls[gm.ballAmount].friction = 0.5;
                            gm.createBall(mousePos, shouldBeStatic, true);
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
                        case(4):
                        {
                            vec2 mousepos = vec2(mousePos.x,mousePos.y);
                            for(int b=0; b<gm.ballAmount; b++)
                            {
                                float dist = gm.getDist(mousepos, gm.balls[b].sprite.getPosition());
                                if(dist < gm.balls[b].sprite.getRadius())
                                    gm.balls[b].isStatic = !gm.balls[b].isStatic;
                            }
                        }
                        break;
                        case(7):
                        {
                            double rad = 1/(180/3.14);

                            vec2 origin = vec2(mousePos.x, mousePos.y);
                            int index = gm.ballAmount;
                            gm.createBall(origin, false, true);
                            gm.balls[gm.ballAmount-1].sprite.setPosition(origin);
                            gm.balls[gm.ballAmount-1].sprite.setRadius(5);
                            gm.balls[gm.ballAmount-1].sprite.setOrigin(5,5);
                            gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                            double ratioAngle = (double)360/points;
                            for(int i = 0; i<points; i++)
                            {
                                gm.createBall(origin, false, true);
                                gm.balls[gm.ballAmount-1].sprite.setPosition(origin.x+sin(gm.ballAmount*ratioAngle*rad) * 50, origin.y+cos(gm.ballAmount*ratioAngle*rad) * 50 );
                                gm.balls[gm.ballAmount-1].sprite.setRadius(5);
                                gm.balls[gm.ballAmount-1].sprite.setOrigin(5,5);
                                gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                                gm.balls[gm.ballAmount-1].friction=1.0;
                                gm.addConstraint(gm.ballAmount-1,gm.ballAmount-2,constraintMode[currentConstraintMode]);
                                gm.addConstraint(gm.ballAmount-1,index,constraintMode[currentConstraintMode]);
                            }
                            gm.addConstraint(index+1, gm.ballAmount-1,constraintMode[currentConstraintMode]);
                        }
                        break;
                        case(11):
                            gm.generateExplosion(mousePos,explosionRad,explosionPower*isExplosionReversed?-1:1);
                            break;
                        }
                    }
                }
                else if(e.mouseButton.button == Mouse::Middle)
                    isMovingCamera = true;
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
                    buttons[4].setTextureRect({shouldBeStatic? 32:16,shouldBeStatic? 64:16,16,16});
                }
                else if(UIselection(buttons[6]))
                    shouldShow = !shouldShow;
                else if(UIselection(buttons[11]))
                {
                    isExplosionReversed = !isExplosionReversed;
                    buttons[11].setTextureRect({isExplosionReversed? 16:32, isExplosionReversed? 64:48,16,16 });
                }
            }

            if(e.type == Event::MouseButtonReleased)
            {
                if(e.mouseButton.button == Mouse::Left)
                {
                    targetIndex = -1;
                    firstBall= -1;
                    currentChain = 0;
                    isHolding= false;
                }
                else if(e.mouseButton.button == Mouse::Middle)
                    isMovingCamera = false;

            }
            if(e.type == Event::KeyPressed && e.key.code == Keyboard::Space)
                isPaused = !isPaused;

        }
        CircleShape preview;
        switch(mode)
        {
        case(2):
            paramText.setString("Constraint mode: " + constraintMode[currentConstraintMode]);
            break;
        case(5):
            paramText.setString("Spacing: " +gm.toString(spacing)+" radius");
            break;
        case(7):
            paramText.setString("Points: " +gm.toString(points));
            break;
        case(8):
            paramText.setString("Mouse Speed:"+gm.toString(cameraSpeed*10));
            break;
        case(9):
            paramText.setString("Spacing: " +gm.toString(spacing)+" radius\nHeight:"+gm.toString(clothHeight));
            break;
        case(11):
            paramText.setString("Explosion Power:"+gm.toString(explosionPower));
            break;
        default:
            paramText.setString("");
            break;
        }
        if(mode==0 || mode == 5|| mode==9)
        {
            preview.setRadius(rad);
            preview.setPosition(mousePos.x,mousePos.y);
            preview.setOrigin(rad,rad);
            preview.setFillColor(Color(50,50,50,50));
        }
        else if(mode==11)
        {
            preview.setRadius(explosionRad);
            preview.setPosition(mousePos.x, mousePos.y);
            preview.setOrigin(explosionRad,explosionRad);
            preview.setFillColor(Color(255,0,0,50));
        }
        if(mode == 3&&isHolding)
            drag();
        else if(mode == 5 && isHolding && !UIselection(ui[0]))
        {
            static int d=0;
            if(firstBall!=-1)
                d = gm.getDist(gm.balls[firstBall].sprite.getPosition(),vec2(mousePos.x, mousePos.y));
            if(d>rad*spacing || firstBall ==-1)
            {

                gm.createBall(mousePos, shouldBeStatic, true);
                if(firstBall != -1)
                    gm.addConstraint(firstBall, gm.ballAmount-1,constraintMode[currentConstraintMode]);
                gm.balls[gm.ballAmount-1].friction = 0.5;
                gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
                gm.balls[gm.ballAmount-1].sprite.setOrigin(vec2(rad,rad));
                firstBall = gm.ballAmount-1;

            }
        }
        else if(mode == 9 && isHolding && !UIselection(ui[0]))
        {
            static int d=0;
            if(firstBall!=-1)
                d = gm.getDist(gm.balls[firstBall].sprite.getPosition(),vec2(mousePos.x, mousePos.y));
            if(d>rad*spacing || firstBall ==-1)
            {

                gm.createBall(mousePos, shouldBeStatic, true);
                currentChain++;
                if(firstBall != -1)
                {
                    gm.addConstraint(firstBall, gm.ballAmount-1,constraintMode[currentConstraintMode]);
                }

                gm.balls[gm.ballAmount-1].friction = 0.5;
                gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
                gm.balls[gm.ballAmount-1].sprite.setOrigin(vec2(rad,rad));
                firstBall = gm.ballAmount-1;
                Vector2f ballPos = gm.balls[firstBall].sprite.getPosition();
                for(int i = 0; i<clothHeight; i++)
                {
                    gm.createBall(Vector2f(ballPos.x,ballPos.y +spacing*rad*(i+1)),shouldBeStatic,true);
                    gm.balls[gm.ballAmount-1].friction = 0.5;
                    gm.balls[gm.ballAmount-1].sprite.setFillColor(Color::Black);
                    gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
                    gm.balls[gm.ballAmount-1].sprite.setOrigin(vec2(rad,rad));
                    gm.addConstraint(gm.ballAmount-2,gm.ballAmount-1,constraintMode[currentConstraintMode]);

                }

                if(currentChain>1)
                {
                    for(int j = 0; j<clothHeight; j++)
                    {
                        cout << gm.ballAmount-(clothHeight-j)<<endl;
                        gm.addConstraint(gm.ballAmount-(clothHeight*2)+j-1,gm.ballAmount-clothHeight+j,constraintMode[currentConstraintMode]);
                    }
                }
            }
        }

        if(!isPaused)
            gm.applyConstraints(maxThreads);

        window.clear(Color::White);
        window.setTitle("Physics Playground FPS:" + gm.toString(fps()));
//DRAW HERE

        window.setView(camera);
        applyUIAnchors();
        window.draw(paramText);
        for(int i =0; i<gm.ballAmount; i++)
        {

            if(!isPaused)
            {
                gm.balls[i].acc.y +=98.8;
                gm.balls[i].upDatePos((float)1/60);
            }

            if(gm.balls[i].sprite.getPosition().y >= 1080-gm.balls[i].sprite.getRadius())
            {
                gm.balls[i].sprite.setPosition(gm.balls[i].sprite.getPosition().x,1080-gm.balls[i].sprite.getRadius());
                gm.balls[i].updateFriction();

            }
            else
            {
                gm.balls[i].acc.x = 0;
            }



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


        if(mode==0 || mode==5 || mode==9 || mode==11)
            window.draw(preview);
        int selected =getSelectedBall();
        if(selected>-1)
        {
            int selectedBallRadius =gm.balls[selected].sprite.getRadius();
            CircleShape selectionCircle = CircleShape(selectedBallRadius);
            selectionCircle.setFillColor(Color::Green);
            selectionCircle.setOrigin(selectedBallRadius,selectedBallRadius);
            selectionCircle.setPosition(gm.balls[selected].sprite.getPosition());
            window.draw(selectionCircle);
            selectionCircle.setRadius(selectedBallRadius-3);
            selectionCircle.setFillColor(Color::Black);
            selectionCircle.setOrigin(selectedBallRadius-3,selectedBallRadius-3);
            window.draw(selectionCircle);
        }
        for(auto& r: gm.rects)
            window.draw(r);
        for(RectangleShape u: ui)
            window.draw(u);
        for(RectangleShape b:buttons)
            window.draw(b);
        window.display();

    }

    return 0;
}
void addConstraint()
{
    if(firstElement >-1)  // if firstElement already assigned
    {
        if(getSelectedBall() ==-1)
            return;
        else if(getSelectedBall() != firstElement)
            secondElement =  getSelectedBall();
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

        if(getSelectedBall() ==-1)
        {
            return;
        }
        else
        {
            firstElement = getSelectedBall();
        }
    }
}
void removeConstraint()
{
    static int firstElement = -1; // default value
    static int secondElement = -1;

    if(firstElement !=-1)  // if firstElement already assigned
    {
        if(getSelectedBall() ==-1)
        {
            return;
        }
        else
        {
            secondElement = getSelectedBall();

        }
        gm.removeConstraint(firstElement,secondElement);
        firstElement = -1;
        secondElement = -1;
    }
    else if(getSelectedBall()!=-1)
        firstElement = getSelectedBall();

}
int getSelectedBall()  // if ball is found return i else return 0, true index = i - 1
{
    for(int i = 0; i<gm.ballAmount; i++)
    {
        if(gm.getDist(vec2(mousePos.x,mousePos.y), gm.balls[i].sprite.getPosition()) < gm.balls[i].sprite.getRadius())
        {

            return i;
        }
    }
    return -1;
}
bool UIselection(RectangleShape rect)
{

    if(mousePos.x >= rect.getPosition().x
            && mousePos.x<= rect.getPosition().x+rect.getSize().x
            && mousePos.y >= rect.getPosition().y
            && mousePos.y <= rect.getPosition().y+rect.getSize().y)

        return true;
    return false;
}
void drag()
{
    if(isHolding && targetIndex == -1) //if target is not defined
    {
        for(int i =0; i<gm.ballAmount; i++)
        {
            int dist = gm.getDist(Vector2f(mousePos.x, mousePos.y), gm.balls[i].sprite.getPosition());

            if( dist < gm.balls[i].sprite.getRadius()*2)
            {
                targetIndex = i;
            }
        }
    }
    else if(isHolding && targetIndex != -1)
        //gm.balls[targetIndex].acc=vec2((mousePos.x-gm.balls[targetIndex].sprite.getPosition().x)/2, (mousePos.y-gm.balls[targetIndex].sprite.getPosition().y)/2);
        gm.balls[targetIndex].sprite.move((mousePos.x-gm.balls[targetIndex].sprite.getPosition().x)/2, (mousePos.y-gm.balls[targetIndex].sprite.getPosition().y)/2);
    else if(!isHolding && targetIndex !=-1)
    {
        gm.balls[targetIndex].acc = Vector2f(0,0);
        targetIndex = -1;
    }
}
float clamp(float minv, float maxv, float v)
{
    if(v<minv)
        return minv;
    else if(v>maxv)
        return maxv;
    else
        return v;
}
vec2 grid(int x,int y, int w)
{
    vec2 origin = vec2(100,50);
    y+=round(x/2);
    return vec2(origin.x +70*(x%w), origin.y+70*y);
}
int fps()
{
    static int t1 = clock();
    static int fps= 0;
    static int lastFps;

    if(t1 - clock() <=-1000)
    {
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
bool isMouseOnUI()
{
    for(auto& u: ui)
        if(UIselection(u))
            return true;
    return false;
}
void applyUIAnchors()
{
    Vector2f origin = window.mapPixelToCoords(Vector2i(0,0));
    ui[0].setPosition(origin);
    ui[1].setPosition(window.mapPixelToCoords(Vector2i(0,1080)).x,1080);
    ui[0].setSize(Vector2f(camera.getSize().x/(940/200),camera.getSize().y));
    ui[1].setSize(Vector2f(camera.getSize().x, camera.getSize().y));
    paramText.setPosition(window.mapPixelToCoords(Vector2i((940),paramText.getCharacterSize())));
    paramText.setScale(Vector2f(camera.getSize().x / 940, camera.getSize().y/540));
    Vector2f ratioSize = Vector2f(940/60,540/60);

    for(int i = 0; i<buttons.size(); i++)
    {
        buttons[i].setSize(Vector2f(camera.getSize().x/ratioSize.x,camera.getSize().y / ratioSize.y));
        buttons[i].setPosition(Vector2f(origin.x+ui[0].getSize().x/2*(i%2),
                                        origin.y+ui[0].getSize().y/ceil((float)buttonCount/2)*floor(i/2)));
    }
}
