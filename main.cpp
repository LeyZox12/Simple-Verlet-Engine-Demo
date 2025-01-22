#include <iostream>
#include <SFML/Graphics.hpp>
#include "../../class/physicsEngine.h"
#include "../../class/UIutils.h"
#include <time.h>
#include <vector>
#include <fstream>

using namespace sf;
using namespace std;

UIutils uiUtil;
physicsEngine gm;
RenderWindow window(VideoMode::getDesktopMode(), "PlayGround");
Event e;
Font font;
Texture texture;
int mode=0;
int rad = 15;
int maxThreads = 10;
int fps();
int currentChain=0;
int spacing = 2;
int points = 20;
int clothHeight = 5;
int getSelectedBall();
int firstBall = -1;
int firstElement = -1;
int secondElement= -1;
int targetIndex = -1;
int buttonCount = 17;
int contraptionIndex = 0;
int currentConstraintMode = 0;
float cameraSpeed=0.5;
float explosionRad = 50.0;
float explosionPower = 50.0;
float motorSpeed = 10.0;
float clamp(float minv, float maxv, float v);
bool isHolding;
bool isMouseOnUI();
bool UIselection(RectangleShape rect);
bool isPaused;
bool shouldShow = true;
bool shouldBeStatic= false;
bool isMovingCamera = false;
bool isExplosionReversed = false;
bool selectionOriginDefined = false;
bool showScript = false;
void addConstraint();
bool isContraptionValid();
void removeConstraint();
void drag();
void start();
void applyUIAnchors();
void multSelect();
void initialize();
void drawOutline(int index);
void saveContraption();
void loadContraption(Vector2f offset, bool isPreview);
void onLeftClick();
void onClick();
void onHold();
Vector2f grid(int x,int y, int w);
Vector2f mousePos;
Vector2f getTextureRect(int x, int y);
Vector2f mouseDelta;
Color defaultColor = Color::Black;
vector<int> multSelection;
vector<RectangleShape> buttons;
vector<string> constraintMode = {"Rigid","Rope", "Spring", "Rigid(spin)"};
string buffer;
Text paramText;
RectangleShape cur(Vector2f(5,5));
RectangleShape ui[2] = {RectangleShape(Vector2f(300,540)), RectangleShape(Vector2f(960,100))};
RectangleShape selectionRect;
View camera(FloatRect(0,0,960,540));
physicsEngine::customConstraintScript testScript;

void start()
{
    testScript.addNode("add");
    testScript.nodes[0].inputIndexes.push_back(2);
    ifstream config("res/config.ini");
    if(config.good())
    {
        config >> buffer >> maxThreads >>
                  buffer >> gm.subSteps >>
                  buffer >> gm.constraintStrength >>
                  buffer >> gm.springStrength >> buffer>>
                  buffer >> defaultColor.r >>
                  buffer >> defaultColor.g >>
                  buffer >> defaultColor.b;
    cout << defaultColor.r << endl << defaultColor.g << defaultColor.b<< endl;
    }
    else
    {
        ofstream config("res/config.ini");
        config << "Threads(def:10)= "
               << 10 << endl
               << "SubDivisions(def:4)= "
               << 4 << endl
               << "ConstraintStrength(def:0.2)= "
               << 0.2 << endl
               << "SpringStrength(def:0.01)= "
               <<0.01 << endl
               << "DefaultColor= r: 0 g: 0 b: 0";
    }
    camera.move(0,750);
    if(!font.loadFromFile("res/font.ttf"))
        cout <<"Error, could not load font.ttf\n";
    if(!texture.loadFromFile("res/spriteSheet.png"))
        cout<<"Error, could not load spriteSheet.png\n";
    paramText.setPosition(0,0);
    paramText.setFont(font);
    paramText.setColor(defaultColor);
    cur.setFillColor(Color::Green);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(true);

    for(int i = 0; i<buttonCount; i++)
        buttons.emplace_back(RectangleShape(Vector2f(60,60)));

    for(auto& u: ui)
        u.setFillColor(defaultColor);
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
        b.setSize(Vector2f(b.getSize().x-15, b.getSize().y-15));
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
            testScript.updateUI(e, window);
            if(e.type == Event::Closed)
                window.close();
            if(e.type == Event::MouseMoved)
            {
                if(isMovingCamera && mode == 8)
                {
                    Vector2f delta = window.mapPixelToCoords(Vector2i(Mouse::getPosition(window).x,Mouse::getPosition(window).y)) - mousePos;
                    camera.move(delta*-cameraSpeed);
                }
                Vector2f newPos = window.mapPixelToCoords(Vector2i(Mouse::getPosition(window).x,Mouse::getPosition(window).y));
                mouseDelta = Vector2f(newPos - mousePos);
                mousePos = newPos;

            }
            if(e.type == Event::KeyReleased)
            {
                int selected = getSelectedBall();
                if(e.key.code == Keyboard::F11)
                {

                }
                if(e.key.code == Keyboard::Delete)
                {
                    for(int i = 0; i < gm.rectAmount; i++)
                        if(UIselection(gm.rects[i]))
                        {
                            gm.deleteRect(i);
                        }
                    if(selected>-1)
                        gm.removeBall(selected);
                    for(int s = 0; s < multSelection.size(); s++)
                        gm.removeBall(multSelection[s]);
                    multSelection.clear();
                }
                if(e.key.code == Keyboard::R && mode == 10)
                    gm.createRect(selectionRect.getPosition(), selectionRect.getSize());
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
                        currentConstraintMode = currentConstraintMode == 0 ? 3 : currentConstraintMode-1;
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
                        motorSpeed-=1;
                        break;
                    case(14):
                    case(15):
                        if(contraptionIndex>0)
                            contraptionIndex--;
                        break;
                    }
                if(e.key.code == Keyboard::Right)
                    switch(mode)
                    {
                    case(2):
                        currentConstraintMode = (currentConstraintMode+1)%4;
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
                        spacing+=1;
                        break;
                    case(11):
                        explosionPower+=10;
                        break;
                    case(12):
                        motorSpeed++;
                        break;
                    case(14):
                    case(15):
                        contraptionIndex++;
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
                    onLeftClick();
                }
                else if(e.mouseButton.button == Mouse::Middle)
                    isMovingCamera = true;
                else if(e.mouseButton.button == Mouse::Right)
                {
                    int selected = getSelectedBall();
                    if(selected>-1)
                        gm.balls[selected].shouldShowStats = !gm.balls[selected].shouldShowStats;
                }
                int i =0;
                for(auto& b: buttons)
                {
                    if(UIselection(b))
                        mode = i;
                    i++;
                }
                int yPos = 96;
                if(UIselection(buttons[4]))
                {
                    shouldBeStatic = !shouldBeStatic;
                    buttons[4].setTextureRect({shouldBeStatic? 32:16,shouldBeStatic? yPos:16,16,16});
                }
                else if(UIselection(buttons[6]))
                    shouldShow = !shouldShow;
                else if(UIselection(buttons[11]))
                {
                    isExplosionReversed = !isExplosionReversed;
                    buttons[11].setTextureRect({isExplosionReversed? 16:32, isExplosionReversed? yPos:48,16,16 });
                }
                else if(UIselection(buttons[13]))
                {
                    int bAmount = gm.ballAmount;
                    for(int i = 0; i < bAmount; i++)
                    {
                        gm.removeBall(i);
                        gm.ballAmount--;
                    }
                    for(int i = 0; i < gm.rectAmount; i++)
                        gm.deleteRect(i);
                    multSelection.clear();
                }
                else if(UIselection(buttons[15]) && multSelection.size() > 0)
                    saveContraption();
                else if(UIselection(buttons[16]))
                    showScript = !showScript;
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
        case(12):
            paramText.setString("Rotation Speed:"+gm.toString(motorSpeed));
            break;
        case(14):
        case(15):
            paramText.setString("Selected Contraption:" + gm.toString(contraptionIndex) + (isContraptionValid() ?"" : "(empty)"));
            break;
        default:
            paramText.setString("");
            break;
        }
        onHold();
        if(!isPaused)
            gm.applyConstraints(gm.ballAmount > maxThreads ? maxThreads : 1);
        window.clear(Color::White);
        window.setTitle("Physics Playground FPS:" + gm.toString(fps()));
        window.setView(camera);
        applyUIAnchors();
        window.draw(paramText);
        for(int r = 0; r < gm.rectAmount; r++)
            window.draw(gm.rects[r]);
        for(int i =0; i<gm.ballAmount; i++)
        {
            if(gm.balls[i].shouldShowStats)
                gm.balls[i].showStats(window,mousePos,font);
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
                line[0].color = defaultColor;
                line[1].color = defaultColor;
                line[0].position = gm.balls[i].sprite.getPosition();
                line[1].position = gm.balls[gm.balls[i].anchorPointsIndex[c]].sprite.getPosition();
                window.draw(line);
            }
            if(shouldShow)
                window.draw(gm.balls[i].sprite);
            gm.balls[i].acc.y *=0.9;
        }
        switch(mode)
        {
            case(0):
            case(5):
            case(9):
                preview.setRadius(rad);
                preview.setPosition(mousePos.x,mousePos.y);
                preview.setOrigin(rad,rad);
                preview.setFillColor(Color(50,50,50,50));
                window.draw(preview);
                break;
            case(11):
                preview.setRadius(explosionRad);
                preview.setPosition(mousePos.x, mousePos.y);
                preview.setOrigin(explosionRad,explosionRad);
                preview.setFillColor(Color(255,0,0,50));
                window.draw(preview);
                break;
            case(14):
                loadContraption(mousePos, true);
                break;
        }

        if(mode == 10 && isHolding)
            window.draw(selectionRect);
        int selected =getSelectedBall();
        for(auto& s : multSelection)
            drawOutline(s);
        if(selected>-1)
        {
            drawOutline(selected);
        }
        for(RectangleShape u: ui)
            window.draw(u);
        for(RectangleShape b:buttons)
            window.draw(b);
        if(showScript)
            testScript.showScriptVisual(window, font, ui[0].getPosition() + Vector2f(400, 0), Vector2f(400, 400));
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
        if(gm.getDist(Vector2f(mousePos.x,mousePos.y), gm.balls[i].sprite.getPosition()) < gm.balls[i].sprite.getRadius())
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
    if(isHolding && targetIndex == -1)
    {
        for(int i =0; i<gm.ballAmount; i++)
        {
            int dist = gm.getDist(Vector2f(mousePos.x, mousePos.y), gm.balls[i].sprite.getPosition());

            if( dist < gm.balls[i].sprite.getRadius())
            {
                targetIndex = i;
            }
        }
    }
    else if(isHolding && targetIndex != -1)
    {
        gm.balls[targetIndex].sprite.move((mousePos.x-gm.balls[targetIndex].sprite.getPosition().x)/2, (mousePos.y-gm.balls[targetIndex].sprite.getPosition().y)/2);
    }
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
Vector2f grid(int x,int y, int w)
{
    Vector2f origin = Vector2f(100,50);
    y+=round(x/2);
    return Vector2f(origin.x +70*(x%w), origin.y+70*y);
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
void multSelect()
{
    if(!selectionOriginDefined)
    {
        selectionRect.setPosition(mousePos.x, mousePos.y);
        selectionRect.setSize(Vector2f(0,0));
        selectionOriginDefined = true;
    }
    else
    {
        Vector2f diff = Vector2f(mousePos.x - selectionRect.getPosition().x,
                                 mousePos.y - selectionRect.getPosition().y);
        selectionRect.setSize(Vector2f(diff.x,diff.y));
        multSelection.clear();
        for(int i = 0; i < gm.ballAmount; i++)
        {
            if(gm.ballRectCollision(gm.balls[i].sprite,selectionRect))
                multSelection.push_back(i);
        }
    }
    selectionRect.setFillColor(Color(0,0,0,50));
}
void drawOutline(int index)
{
    int ballRadius =gm.balls[index].sprite.getRadius();
    CircleShape selectionCircle = CircleShape(ballRadius);
    selectionCircle.setFillColor(Color::Green);
    selectionCircle.setOrigin(ballRadius,ballRadius);
    selectionCircle.setPosition(gm.balls[index].sprite.getPosition());
    window.draw(selectionCircle);
    selectionCircle.setRadius(ballRadius-3);
    selectionCircle.setFillColor(defaultColor);
    selectionCircle.setOrigin(ballRadius-3,ballRadius-3);
    window.draw(selectionCircle);
}
void saveContraption()
{
    Vector2f origin = Vector2f(gm.balls[multSelection[0]].sprite.getPosition());
    map<int, int> ballsTransformed;
    for(int i = 0; i < multSelection.size(); i++)
    {
        ballsTransformed.insert(make_pair(multSelection[i],i));
    }
    ofstream file("contraption" + gm.toString(contraptionIndex) + ".contr");
    file << multSelection.size() << endl;
    for(int i = 0; i < multSelection.size(); i++)
    {
        physicsEngine::ball b = gm.balls[multSelection[i]];
        file << b.sprite.getPosition().x - origin.x << endl
             << b.sprite.getPosition().y - origin.y << endl
             << b.sprite.getRadius() << endl
             << b.isStatic << endl
             << b.friction << endl
             << b.rotationSpeed << endl
             << gm.balls[multSelection[i]].anchorCount << endl;
        for(int j = 0; j < gm.balls[multSelection[i]].anchorCount; j++)
        {
            file << ballsTransformed[b.anchorPointsIndex[j]] << endl
                 << b.maxDist[j] << endl
                 << b.constraintMode[j] << endl;
        }
    }
    file.close();
}
bool isContraptionValid()
{
    ifstream file("contraption" + gm.toString(contraptionIndex) + ".contr");
    return file.good();
}
void loadContraption(Vector2f offset, bool isPreview)
{
    ifstream file("contraption" + gm.toString(contraptionIndex) + ".contr");
    int ballCount = 0;
    file >> ballCount;
    Vector2f pos;
    float radius;
    int isStatic;
    int anchorCount;
    int anchorIndex;
    float maxDist;
    float rotationSpeed;
    float friction;
    string cMode;
    int ballAmount = gm.ballAmount;
    for(int i = 0; i < ballCount; i++)
    {
        file >> pos.x >> pos.y >> radius >> isStatic >> friction >> rotationSpeed >> anchorCount;
        if(isPreview)
        {
            CircleShape preview(radius);
            preview.setPosition(Vector2f(pos+offset));
            preview.setOrigin(radius,radius);
            preview.setFillColor(Color(0,0,0,50));
            window.draw(preview);
            for(int j = 0; j < anchorCount; j++)
            {
                file >> anchorIndex >> maxDist >> cMode;
            }
        }
        else
        {
            gm.createBall(Vector2f(pos+offset),isStatic == 1 ? true : false, true);
            gm.balls[gm.ballAmount-1].sprite.setRadius(radius);
            gm.balls[gm.ballAmount-1].sprite.setOrigin(radius,radius);
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].friction = friction;
            gm.balls[gm.ballAmount-1].rotationSpeed = rotationSpeed;
            for(int j = 0; j < anchorCount; j++)
            {
                bool shouldAddConstraint = true;
                file >> anchorIndex >> maxDist >> cMode;
                for(int c = 0; c < gm.balls[gm.ballAmount-1].anchorCount; c++)
                    if(gm.balls[gm.ballAmount-1].anchorPointsIndex[c] == anchorIndex+ballAmount)
                        shouldAddConstraint = false;
                if(shouldAddConstraint)
                {
                    gm.addConstraint(gm.ballAmount-1, anchorIndex+ballAmount, cMode, maxDist);
                }
            }
        }

    }

}
void onClick()
{

}
void onHold()
{
    if(mode == 3&&isHolding)
        drag();
    else if(mode == 5 && isHolding && !UIselection(ui[0]))
    {
        static int d=0;
        if(firstBall!=-1)
            d = gm.getDist(gm.balls[firstBall].sprite.getPosition(),Vector2f(mousePos.x, mousePos.y));
        if(d>rad*spacing || firstBall ==-1)
        {

            gm.createBall(mousePos, shouldBeStatic, true);
            if(firstBall != -1)
                gm.addConstraint(firstBall, gm.ballAmount-1,constraintMode[currentConstraintMode],spacing*rad);
            gm.balls[gm.ballAmount-1].friction = 0.5;
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
            gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(rad,rad));
            gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
            firstBall = gm.ballAmount-1;
        }
    }
    else if(mode == 9 && isHolding && !UIselection(ui[0]))
    {
        static int d=0;
        if(firstBall!=-1)
            d = gm.getDist(gm.balls[firstBall].sprite.getPosition(),Vector2f(mousePos.x, mousePos.y));
        if(d>rad*spacing || firstBall ==-1)
        {

            gm.createBall(mousePos, shouldBeStatic, true);
            currentChain++;
            if(firstBall != -1)
            {
                gm.addConstraint(firstBall, gm.ballAmount-1,constraintMode[currentConstraintMode],spacing*rad);
            }

            gm.balls[gm.ballAmount-1].friction = 0.5;
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
            gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(rad,rad));
            gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
            firstBall = gm.ballAmount-1;
            Vector2f ballPos = gm.balls[firstBall].sprite.getPosition();
            for(int i = 0; i<clothHeight; i++)
            {
                gm.createBall(Vector2f(ballPos.x,ballPos.y +spacing*rad*(i+1)),shouldBeStatic,true);
                gm.balls[gm.ballAmount-1].friction = 0.5;
                gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
                gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
                gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(rad,rad));
                gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
                gm.addConstraint(gm.ballAmount-2,gm.ballAmount-1,constraintMode[currentConstraintMode],spacing*rad);
            }
            if(currentChain>1)
            {
                for(int j = 0; j<clothHeight; j++)
                {
                    gm.addConstraint(gm.ballAmount-(clothHeight*2)+j-1,gm.ballAmount-clothHeight+j,constraintMode[currentConstraintMode],spacing*rad);
                }
            }
        }
    }
    else if(mode == 10 && isHolding)
    {
        multSelect();
    }
    else if(mode == 16 && isHolding)
    {
        cout << "holding 16" << endl;
        testScript.grab(mousePos);
    }
}
void onLeftClick()
{
    if(!UIselection(ui[0]))
    {
        switch (mode)
        {
        case(0):
            gm.balls[gm.ballAmount].friction = 0.5;
            gm.createBall(mousePos, shouldBeStatic, true);
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].sprite.setRadius(rad);
            gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(rad,rad));
            gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
            break;
        case(1):
            removeConstraint();
            break;
        case(2):
            addConstraint();
            break;
        case(4):
        {
            Vector2f mousepos = Vector2f(mousePos.x,mousePos.y);
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
            Vector2f origin = Vector2f(mousePos.x, mousePos.y);
            int index = gm.ballAmount;
            gm.createBall(origin, false, true);
            gm.balls[gm.ballAmount-1].sprite.setPosition(origin);
            gm.balls[gm.ballAmount-1].sprite.setRadius(5);
            gm.balls[gm.ballAmount-1].sprite.setOrigin(5,5);
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
            double ratioAngle = (double)360/points;
            for(int i = 0; i<points; i++)
            {
                gm.createBall(origin, false, true);
                gm.balls[gm.ballAmount-1].sprite.setPosition(origin.x+sin(gm.ballAmount*ratioAngle*rad) * 50, origin.y+cos(gm.ballAmount*ratioAngle*rad) * 50 );
                gm.balls[gm.ballAmount-1].sprite.setRadius(5);
                gm.balls[gm.ballAmount-1].sprite.setOrigin(5,5);
                gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
                gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
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
        case(14):
            loadContraption(mousePos, false);
            break;
        }
        if(getSelectedBall() == -1)
        {
            selectionOriginDefined=false;
            multSelection.clear();
        }
    }
}
