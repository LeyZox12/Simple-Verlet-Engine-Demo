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
        void UIEventManager(Event e, Vector2f mousePos)
        {

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
                           "\nConstraint count:" + tostr(anchorCount) + "\nStatic:" + staticStr + "\nvelX:" + tostr(vel.x) + "  velY:" + tostr(vel.y));
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
            6:Sqrt
            7:Atan2
            8:sin
            9:cos
            10:tan
            11:round
            12:x1
            13:y1
            14:x2
            15:y2
            16:const
            17:time
            18:pythagorean theorem
            19:abs
            20:pi
            21:out
            */
            public:
                int index;
                int inputCount;
                int outputCount;
                int type;
                float value;
                string label;
                vector<int> inputIndexes;
                vector<int> outputIndexes;
                vector<string> outputTypes;
                Vector2f output1;
                Vector2f output2;
                Vector2f offset = Vector2f(0, 0);
                Text name;
                RectangleShape scriptVisual;
                node()
                {
                    value = 0;
                }
                void setType(int type)
                {
                    this->type = type;
                    switch(type)
                    {
                        case(0):
                            label = "Add";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(1):
                            label = "Sub";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(2):
                            label = "Mult";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(3):
                            label = "Div";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(4):
                            label = "Pow";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(5):
                            label = "Mod";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(6):
                            label = "Sqrt";
                            inputCount = 1;
                            outputCount = 1;
                            break;
                        case(7):
                            label = "Atan2";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(8):
                            label = "Sin";
                            inputCount = 1;
                            outputCount = 1;
                            break;
                        case(9):
                            label = "Cos";
                            inputCount = 1;
                            outputCount = 1;
                            break;
                        case(10):
                            label = "Tan";
                            inputCount = 1;
                            outputCount = 1;
                            break;
                        case(11):
                            label = "Round";
                            inputCount = 1;
                            outputCount = 1;
                            break;
                        case(12):
                            label = "x1";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(13):
                            label = "y1";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(14):
                            label = "x2";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(15):
                            label = "y2";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(16):
                            label = "const";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(17):
                            label = "Time";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(18):
                            label = "Pyth";
                            inputCount = 2;
                            outputCount = 1;
                            break;
                        case(19):
                            label = "Abs";
                            inputCount = 1;
                            outputCount = 1;
                        case(20):
                            label = "pi";
                            inputCount = 0;
                            outputCount = 1;
                            break;
                        case(21):
                            label = "Out";
                            inputCount = 2;
                            outputCount = 0;
                            break;
                    }
                    for(int i = 0; i < inputCount; i++)
                    {
                        CircleShape inputCircle;
                    }
                }
            float getValue(vector<node> n)
            {
                vector<float> inputs;
                for(auto& i : inputIndexes)
                    inputs.push_back(n[i].getValue(n));
                switch(type)
                {
                    case(0):
                        return inputs[0] + inputs[1];
                        break;
                    case(1):
                        return inputs[0] - inputs[1];
                        break;
                    case(2):
                        return inputs[0] * inputs[1];
                        break;
                    case(3):
                        return inputs[0] / inputs[1];
                        break;
                    case(4):
                        return pow(inputs[0], inputs[1]);
                        break;
                    case(5):
                        return fmod(inputs[0], inputs[1]);
                        break;
                    case(6):
                        return sqrt(inputs[0]);
                        break;
                    case(7):
                        return atan2(inputs[0], inputs[1]);
                        break;
                    case(8):
                        return sin(inputs[0]);
                        break;
                    case(9):
                        return cos(inputs[0]);
                        break;
                    case(10):
                        return tan(inputs[0]);
                        break;
                    case(11):
                        return round(inputs[0]);
                        break;
                    case(12):
                        return value;
                        break;
                    case(13):
                        return value;
                        break;
                    case(14):
                        return value;
                        break;
                    case(15):
                        return value;
                        break;
                    case(16):
                        return value;
                        break;
                    case(17):
                        return clock();
                    case(18):
                        return sqrt(inputs[0] * inputs[0] + inputs[1] * inputs[1]);
                        break;
                    case(19):
                        return abs(inputs[0]);
                        break;
                    case(20):
                        return 3.1415926;
                }
            }
            Vector2f getOutput(vector<node> n, Vector2f pos1, Vector2f pos2)
            {
                for(int i = 0; i < n.size(); i++)
                {
                    if(n[i].type == 12) n[i].value = pos1.x;
                    if(n[i].type == 13) n[i].value = pos1.y;
                    if(n[i].type == 14) n[i].value = pos2.x;
                    if(n[i].type == 15) n[i].value = pos2.y;
                }
                if(label == "Out")
                {
                    Vector2f out = Vector2f(n[inputIndexes[0]].getValue(n), n[inputIndexes[1]].getValue(n));
                    return out;
                }
                raise(1);
            }
        };
        vector<node> nodes;
        node out;
        UIutils ui;
        int nodeSizeX = 50;
        int centerOffset = 30;
        int grabbedIndex = -1;
        int inputLinkIndex = -1;
        int currentIndex = 0;
        int outputLinkIndex = -1;
        Vector2f grabbedOffset;
        Vector2f currentPos;
        Vector2f currentSize;
        Vector2f currentMousePos;
        vector<RectangleShape> rectangles;
        vector<string> nodeLabels = {"Add", "Sub", "Mult", "Div", "Pow", "Mod", "Sqrt","Atan2", "Sin", "Cos", "Tan", "Round", "x1", "y1", "x2", "y2", "Const", "Time", "Pyth", "Pi"};
        customConstraintScript()
        {

        }
        void unLink()
        {
            for(int i = 0; i < nodes.size(); i++)
            {
                if(inputLinkIndex == -1)
                    for(int n = 0; n < nodes[i].inputCount; n++)
                    {
                        Vector2f inputCirclePos = rectangles[i].getPosition() + Vector2f(-5, centerOffset + 10 * n - 5);
                        Vector2f diff = inputCirclePos - currentMousePos;
                        float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
                        if(dist < 10)
                        {
                            inputLinkIndex = i;
                        }
                    }
                if(outputLinkIndex == -1)
                    for(int o = 0; o < nodes[i].outputCount; o++)
                    {

                        Vector2f outputCirclePos = rectangles[i].getPosition() + Vector2f(nodeSizeX + 5, centerOffset + 10 * o - 5);
                        Vector2f diff = outputCirclePos - currentMousePos;
                        float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
                        if(dist < 10)
                        {
                            outputLinkIndex = i;
                        }
                    }
            }
            if(inputLinkIndex >= 0 && outputLinkIndex >= 0 && inputLinkIndex != outputLinkIndex)
            {
                vector<int>& inputIndexes = nodes[inputLinkIndex].inputIndexes;
                inputIndexes.erase(find(inputIndexes.begin(), inputIndexes.end(), outputLinkIndex));
                inputLinkIndex = -1;
                outputLinkIndex = -1;
            }
        }
        void link()
        {
            for(int i = 0; i < nodes.size(); i++)
            {
                if(inputLinkIndex == -1)
                    for(int n = 0; n < nodes[i].inputCount; n++)
                    {
                        Vector2f inputCirclePos = rectangles[i].getPosition() + Vector2f(-5, centerOffset + 10 * n );
                        Vector2f diff = inputCirclePos - currentMousePos;
                        float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
                        if(dist < 10)
                        {
                            inputLinkIndex = i;
                        }
                    }
                if(outputLinkIndex == -1)
                    for(int o = 0; o < nodes[i].outputCount; o++)
                    {

                        Vector2f outputCirclePos = rectangles[i].getPosition() + Vector2f(nodeSizeX + 5, centerOffset + 10 * o);
                        Vector2f diff = outputCirclePos - currentMousePos;
                        float dist = sqrt(diff.x * diff.x + diff.y * diff.y);
                        if(dist < 10)
                        {
                            outputLinkIndex = i;
                        }
                    }
            }
            if(inputLinkIndex >= 0 && outputLinkIndex >= 0 && inputLinkIndex != outputLinkIndex)
            {
                nodes[outputLinkIndex].outputIndexes.push_back(inputLinkIndex);
                nodes[inputLinkIndex].inputIndexes.push_back(outputLinkIndex);
                inputLinkIndex = -1;
                outputLinkIndex = -1;
            }
        }
        void init(Font uiFont)
        {
            out.setType(21);
            out.offset = Vector2f(200, 100);
            nodes.push_back(out);

            ui.font = uiFont;
            ui.addDropDown(currentPos, Vector2f(100,50), nodeLabels, 3, "Nodes");
            ui.addButton(currentPos + Vector2f(100, 0), Vector2f(100,50),
                         [this]
                         {
                            if(ui.dropDowns[0].value != "Nodes")
                            {
                                node newNode;
                                newNode.index = nodes.size();
                                newNode.setType(ui.dropDowns[0].valIndex);
                                newNode.offset = Vector2f(currentSize.x / 2, currentSize.y / 2);
                                nodes.push_back(newNode);
                            }
                         } , "Create");
            ui.addButton(currentPos + Vector2f(200, 0), Vector2f(100, 50),
                         [this]
                         {
                            stringstream ss;
                            string out;
                            ss << currentIndex;
                            ss >> out;

                            ofstream file("res/customConstraint" + out + ".constr");
                            file << nodes.size() << endl;
                            for(auto& n : nodes)
                            {
                                file << n.type << endl << n.offset.x << endl << n.offset.y << endl << n.value << endl << n.inputIndexes.size() << endl;
                                for(int input = 0; input < n.inputIndexes.size(); input++)
                                        file << n.inputIndexes[input] << endl;
                                file << n.outputIndexes.size() << endl;
                                for(int output = 0; output < n.outputIndexes.size(); output++)
                                    file << n.outputIndexes[output] << endl;
                            }
                            file.close();
                         }, "Save");
            ui.addButton(currentPos + Vector2f(300, 0), Vector2f(100, 50),
                         [this]
                         {
                            stringstream ss;
                            string out;
                            ss << currentIndex;
                            ss >> out;
                            ifstream file("res/customConstraint" + out + ".constr");

                            if(file.good())
                            {
                                nodes.clear();
                                int nodeAmount;
                                file >> nodeAmount;
                                for(int i = 0; i < nodeAmount; i++)
                                    nodes.push_back(node());
                                for(int i = 0; i < nodeAmount; i++)
                                {

                                    int type;
                                    float x;
                                    float y;
                                    float val;
                                    int inputCount;
                                    int outputCount;
                                    file >> type >> x >> y >> val >> inputCount;

                                    nodes[i].offset = Vector2f(x, y);
                                    nodes[i].value = val;
                                    for(int input = 0; input < inputCount; input++)
                                    {
                                        int inputIndex;
                                        file >> inputIndex;
                                        nodes[i].inputIndexes.push_back(inputIndex);
                                        nodes[inputIndex].outputIndexes.push_back(i);
                                    }
                                    file >> outputCount;
                                    for(int output = 0; output < outputCount; output++)
                                    {
                                        int outputIndex;
                                        file >> outputIndex;
                                        nodes[i].outputIndexes.push_back(outputIndex);

                                    }
                                    nodes[i].setType(type);
                                    nodes[i].index = i;
                                }
                            }
                            file.close();
                         }, "Load");
            ui.addButton(currentPos + Vector2f(400, 0), Vector2f(100, 50),
                         [this]
                         {
                             nodes.clear();
                             nodes.push_back(out);
                         }, "Clear");
        }
        void addNode(int type)
        {
            node newNode;
            newNode.index = nodes.size();
            newNode.setType(type);
            newNode.offset = Vector2f(currentSize.x / 2, currentSize.y / 2);
            nodes.push_back(newNode);
        }
        void removeNode()
        {
            for(int i = 0; i < nodes.size(); i++)
            {
                if(isHovering(i))
                {
                    int lastIndex = nodes.size() - 1;
                    node& last = nodes[lastIndex];
                    for(int j = 0; j < nodes[i].inputIndexes.size(); j++)
                        nodes[nodes[i].inputIndexes[j]].outputIndexes.erase(find(nodes[nodes[i].inputIndexes[j]].outputIndexes.begin(), nodes[nodes[i].inputIndexes[j]].outputIndexes.end(), i));
                        cout << nodes[i].outputIndexes.size() << endl;
                    for(int j = 0; j < nodes[i].outputIndexes.size(); j++)
                                                nodes[nodes[i].outputIndexes[j]].inputIndexes.erase(find(nodes[nodes[i].outputIndexes[j]].inputIndexes.begin(), nodes[nodes[i].outputIndexes[j]].inputIndexes.end(), i));
                    nodes[i].inputIndexes.clear();
                    nodes[i].outputIndexes.clear();
                    for(int j = 0; j < last.inputIndexes.size(); j++)
                        replace(nodes[last.inputIndexes[j]].inputIndexes.begin(),
                                nodes[last.inputIndexes[j]].inputIndexes.end(), lastIndex, i);
                    for(int j = 0; j < last.outputIndexes.size(); j++)
                        replace(nodes[last.outputIndexes[j]].outputIndexes.begin(),
                                nodes[last.outputIndexes[j]].outputIndexes.end(), lastIndex, i);
                    nodes[i] = nodes[lastIndex];
                    nodes.pop_back();
                }
            }
        }
        void updateUI(Event e, RenderWindow& window)
        {
            currentMousePos = window.mapPixelToCoords(Vector2i(Mouse::getPosition(window).x,Mouse::getPosition(window).y));
            ui.dropDowns[0].elementsNames = nodeLabels;
            ui.dropDowns[0].pos = currentPos;
            int o = 0;
            for(auto& b : ui.buttons)
            {
                b.pos = currentPos + Vector2f(100 * ++o, 0);
            }
            ui.updateElements(e, window);
            if(e.type == Event::KeyPressed && e.key.code == Keyboard::Left && currentIndex > 0)
                currentIndex--;
            else if(e.type == Event::KeyPressed && e.key.code == Keyboard::Right)
                currentIndex++;
            for(int i = 0; i < nodes.size(); i++)
            {
                if(e.type == Event::KeyPressed && e.key.code == Keyboard::Up)
                {
                if(nodes[i].label == "const" && isHovering(i))
                        nodes[i].value++;


                }
                else if(e.type == Event::KeyPressed && e.key.code == Keyboard::Down)
                {
                    if(nodes[i].label == "const" && isHovering(i))
                        nodes[i].value--;
                }
            }
            if(e.type == Event::MouseButtonReleased)
            {
                grabbedIndex = -1;
            }
        }
        bool isHovering(int index)
        {
            Vector2f rectPos = currentPos + nodes[index].offset;
            Vector2f rectSize = Vector2f(nodeSizeX, centerOffset + 10 * nodes[index].inputCount);
            return (currentMousePos.x > rectPos.x &&
                    currentMousePos.x < rectPos.x + rectSize.x &&
                    currentMousePos.y > rectPos.y &&
                    currentMousePos.y < rectPos.y + rectSize.y);
        }
        void grab(Vector2f mousePos)
        {
            if(grabbedIndex == -1)
                for(int i = 0; i < nodes.size(); i++)
                {
                    Vector2f rectPos = currentPos + nodes[i].offset;
                    Vector2f rectSize = Vector2f(nodeSizeX, centerOffset + 10 * nodes[i].inputCount);
                    if(isHovering(i))
                    {
                        grabbedIndex = i;
                        grabbedOffset = Vector2f(mousePos - rectPos);
                        break;
                    }
                }
            if(grabbedIndex > -1)
                nodes[grabbedIndex].offset = Vector2f(currentMousePos - currentPos - grabbedOffset);
        }
        void showScriptVisual(RenderWindow& window, Font font, Vector2f pos, Vector2f sizeR)
        {
            Text label;

            label.setCharacterSize(20);
            label.setScale(0.5, 0.5);
            label.setFont(font);
            label.setPosition(pos.x + 400, pos.y + 20);

            stringstream ss;
            string out;
            ss << currentIndex;
            ss >> out;
            if(ifstream("res/customConstraint" + out + ".constr").good())
                label.setString("ConstraintIndex:" + out);
            else
                label.setString("ConstraintIndex:" + out + "(empty)");
            ui.font = font;
            currentPos = pos;
            currentSize = sizeR;
            RectangleShape scriptRect;
            rectangles.clear();
            scriptRect.setPosition(pos.x, pos.y);
            scriptRect.setSize(Vector2f(window.getSize().x, window.getSize().y));
            scriptRect.setFillColor(Color(50, 50, 50, 200));
            window.draw(scriptRect);
            window.draw(label);
            for(int i = 0; i < nodes.size(); i++)
            {
                RectangleShape nodeRect;
                nodeRect.setPosition(pos + nodes[i].offset);
                nodeRect.setFillColor(Color::Black);
                nodeRect.setSize(Vector2f(nodeSizeX, centerOffset + 10 * nodes[i].inputCount));
                window.draw(nodeRect);
                rectangles.push_back(nodeRect);

                label.setPosition(pos + nodes[i].offset);
                if(nodes[i].label == "const")
                {
                    stringstream ss;
                    string strValue;
                    ss << nodes[i].value;
                    ss >> strValue;
                    label.setString(nodes[i].label + "\nvalue:" + strValue);
                }
                else
                label.setString(nodes[i].label);
                label.setFillColor(Color::White);
                window.draw(label);
                ui.displayElements(window);
            }
            for(int i = 0; i < nodes.size(); i++)
            {
                CircleShape nodeCircle(5);
                nodeCircle.setFillColor(Color(0, 0, 0));
                nodeCircle.setOrigin(5, 5);
                VertexArray line(LineStrip, 2);
                for(int n = 0; n < nodes[i].inputCount; n++)
                {
                    nodeCircle.setPosition(rectangles[i].getPosition() + Vector2f(0, centerOffset + 10 * n));
                    window.draw(nodeCircle);
                    if(nodes[i].inputIndexes.size() > n)
                    {
                        Vector2f rectPos = pos + nodes[nodes[i].inputIndexes[n]].offset;
                        RectangleShape &inputNodeRect = rectangles[nodes[i].inputIndexes[n]];
                        line[0].position = rectPos + Vector2f(inputNodeRect.getSize().x, centerOffset);
                        line[1].position = nodeCircle.getPosition();
                        line[0].color = Color::Black;
                        line[1].color = Color::Black;
                        window.draw(line);
                    }
                }
                for(int o = 0; o < nodes[i].outputCount; o++)
                {
                    nodeCircle.setPosition(rectangles[i].getPosition() + Vector2f(rectangles[i].getSize().x, centerOffset + 10 * o));
                    window.draw(nodeCircle);
                    if(nodes[i].outputIndexes.size() > o)
                    {
                        Vector2f rectPos = pos + nodes[nodes[i].outputIndexes[o]].offset;
                        RectangleShape &outputNodeRect = rectangles[nodes[i].outputIndexes[o]];

                    }
                }
            }
                ui.displayElements(window);
            }
        };

    unsigned int subSteps=4;
    unsigned int ballAmount;
    unsigned int rectAmount;
    float constraintStrength = 0.1;
    float springStrength = 0.01;
    ball balls[20000];
    RectangleShape rects[2000];
    customConstraintScript customConstraint;
    double pi = 3.14159;
    double r = (double)pi/180;
    double d = 180/pi;
protected:

private:
};

#endif
