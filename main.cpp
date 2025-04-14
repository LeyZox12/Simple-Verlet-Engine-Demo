#include <iostream>
#include <SFML/Graphics.hpp>
#include "../../class/physicsEngine.h"
#include "../../class/UIutils.h"
#include <time.h>
#include <vector>
#include <fstream>
#include <map>
#include <lua.hpp>

using namespace sf;
using namespace std;

UIutils uiUtil;
physicsEngine gm;
RenderWindow window(VideoMode::getDesktopMode(), "PlayGround", Style::Titlebar);

Font font;
Texture texture;
int mode=0;
int rad = 15;
int maxThreads = 10;
int fps();
unsigned int currentChain=0;
int spacing = 2;
unsigned int points = 20;
unsigned int clothHeight = 5;
int getSelectedBall();
int firstBall = -1;
int firstElement = -1;
int secondElement= -1;
int targetIndex = -1;
unsigned int buttonCount = 17;
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
void onRightClick();
void onHold();
Vector2f grid(int x,int y, int w);
Vector2f mousePos;
Vector2f getTextureRect(int x, int y);
Vector2f mouseDelta;
Color defaultColor = Color::Black;
vector<int> multSelection;
vector<RectangleShape> buttons;
vector<string> constraintMode = {"Rigid","Rope", "Spring", "Custom"};
string buffer;
RectangleShape cur(Vector2f(5,5));
RectangleShape ui[2] = {RectangleShape(Vector2f(300,540)), RectangleShape(Vector2f(960,100))};
RectangleShape selectionRect;
View camera({0,0},{960,540});

string toStr(float v)
{
    stringstream ss;
    string out;
    ss << v;
    ss >> out;
    return out;
}

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
            string toString(float val)
            {
                stringstream ss;
                string str;
                ss << val;
                ss >> str;
                return str;
            }
            string getCompiled(vector<node> n, int inputIndex)
            {
                if(label == "Out")
                {
                    return n[inputIndexes[inputIndex]].getCompiled(n, -1);
                }
                else if(inputIndex == -1)
                {
                    vector<string> inputs;
                    for(auto& i : inputIndexes)
                        inputs.push_back(n[i].getCompiled(n, -1));
                    switch(type)
                    {
                        case(0):
                            return "(" + inputs[0] + "+" + inputs[1] + ")";
                            break;
                        case(1):
                            return "(" + inputs[0] + "-" + inputs[1] + ")";
                            break;
                        case(2):
                            return "(" + inputs[0] + "*" + inputs[1] + ")";
                            break;
                        case(3):
                            return "(" + inputs[0] + "/" + inputs[1] + ")";
                            break;
                        case(4):
                            return "math.pow(" + inputs[0] + "," + inputs[1] + ")";
                            break;
                        case(5):
                            return "(" + inputs[0] + "%" + inputs[1] + ")";
                            break;
                        case(6):
                            return "math.sqrt(" + inputs[0] + ")";
                            break;
                        case(7):
                            return "math.atan2(" + inputs[0] + "," + inputs[1] + ")";
                            break;
                        case(8):
                            return "math.sin(" + inputs[0] + ")";
                            break;
                        case(9):
                            return "math.cos(" + inputs[0] + ")";
                            break;
                        case(10):
                            return "math.tan(" + inputs[0] + ")";
                            break;
                        case(11):
                            return "r(" + inputs[0] + ")";
                            break;
                        case(12):
                            return "x1";
                            break;
                        case(13):
                            return "y1";
                            break;
                        case(14):
                            return "x2";
                            break;
                        case(15):
                            return "y2";
                            break;
                        case(16):
                            return "" + toString(value) + "";
                            break;
                        case(17):
                            return "time";
                        case(18):
                            return "math.sqrt(" + inputs[0] + "*" + inputs[0] + "+" + inputs[1] + "*" + inputs[1] + ")";

                            break;
                        case(19):
                            return "abs(" + inputs[0] + ")";
                            break;
                        case(20):
                            return "3.141592654";
                    }
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
        bool useSubSteps = true;
        bool showLine = true;
        Vector2f grabbedOffset;
        Vector2f currentPos;
        Vector2f currentSize;
        Vector2f currentMousePos;
        vector<RectangleShape> rectangles;
        vector<string> instructions;
        vector<string> nodeLabels = {"Add", "Sub", "Mult", "Div", "Pow", "Mod", "Sqrt","Atan2", "Sin", "Cos", "Tan", "Round", "x1", "y1", "x2", "y2", "Const", "Time", "Pyth", "Pi"};
        string cmd;
        string vars;
        lua_State *lua = luaL_newstate();

        customConstraintScript()
        {
            luaL_openlibs(lua);
        }

        Vector2f getPos(Vector2f p1, Vector2f p2)
        {


        vars = "x1 =" + toStr(p1.x) + "\ny1 = " + toStr(p1.y)+ "\nx2 = " + toStr(p2.x) + "\ny2 = " + toStr(p2.y) + "\ntime = " + toStr(clock());
        string combined = vars + "\n" + cmd;
        //cout << combined << endl;
        Vector2f result;
        luaL_dostring(lua, combined.c_str());
        lua_pushinteger(lua, 1);
        lua_gettable(lua, -2);
        result.x = (lua_tonumber(lua, -1));
        lua_pop(lua, 1);
        lua_pushinteger(lua, 2);
        lua_gettable(lua, -2);
        result.y = lua_tonumber(lua, -1);
        lua_pop(lua, 1);
        return result;
        //b.sprite.setPosition(customConstraint.nodes[0].getOutput(customConstraint.nodes, b.sprite.getPosition(), gm.balls[b.anchorPointsIndex[c]].sprite.getPosition()));
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
        vector<string> separate(string hayStack, string needle)
        {
            vector<string> separated;
            string current = "";
            bool shouldAdd = false;
            for(auto& c : hayStack)
            {
                if(c == needle && current == "")
                    shouldAdd = true;
                else if(shouldAdd && c != needle)
                    current += c;
                else if(shouldAdd && c == needle)
                {
                    shouldAdd = false;
                    separated.push_back(current);
                    cout << current << endl;
                    current = "";
                }
            }
            return separated;
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
                            file << showLine<< endl << useSubSteps << endl << nodes.size() << endl;
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
                            vars = "x1 = 0\ny1 = 0\nx2 = 0\ny2 = 0\ntime = 0";
                            cmd = "return {" + nodes[0].getCompiled(nodes, 0) + "," + nodes[0].getCompiled(nodes, 1) + "}";
                            cout << "return {" << nodes[0].getCompiled(nodes, 0) << "," << nodes[0].getCompiled(nodes, 1) << "}";

                         }, "Compile");
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
                                int b1;
                                int b2;
                                file >> b1 >> b2;
                                showLine = b1 == 1;
                                useSubSteps = b2 == 1;
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
            ui.addButton(currentPos + Vector2f(500, 0), Vector2f(100, 50), [this]{showLine = !showLine;}, "Line");
            ui.addButton(currentPos + Vector2f(600, 0), Vector2f(100, 50), [this]{useSubSteps = !useSubSteps;}, "Substeps");
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
        void updateUI(const optional<Event> e, RenderWindow& window)
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
            if(e->is<Event::KeyPressed>() && e->getIf<Event::KeyPressed>() -> code == Keyboard::Key::Left && currentIndex > 0)
                currentIndex--;
            else if(e->is<Event::KeyPressed>() && e->getIf<Event::KeyPressed>() -> code == Keyboard::Key::Right)
                currentIndex++;
            for(int i = 0; i < nodes.size(); i++)
            {
                if(e->is<Event::KeyPressed>() && e->getIf<Event::KeyPressed>() -> code == Keyboard::Key::Up)
                {
                if(nodes[i].label == "const" && isHovering(i))
                        nodes[i].value++;


                }
                else if(e->is<Event::KeyPressed>() && e->getIf<Event::KeyPressed>() -> code == Keyboard::Key::Down)
                {
                    if(nodes[i].label == "const" && isHovering(i))
                        nodes[i].value--;
                }
            }
            if(e->is<Event::MouseButtonReleased>())
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
            static Text label(font, "", 20);
            label.setScale(Vector2f(0.5, 0.5));
            label.setPosition(Vector2f(pos.x, pos.y + 70));
            stringstream ss;
            string out;
            ss << currentIndex;
            ss >> out;
            if(ifstream("res/customConstraint" + out + ".constr").good())
                label.setString("selected:" + out);
            else
                label.setString("selected:" + out + "(empty)");
            ui.font = font;
            ui.buttons[4].buttonNameStr = showLine ? "Line" : "NoLine";
            ui.buttons[5].buttonNameStr = useSubSteps ? "Sub" : "NoSub";
            currentPos = pos;
            currentSize = sizeR;
            RectangleShape scriptRect;
            rectangles.clear();
            scriptRect.setPosition(Vector2f(pos.x, pos.y));
            scriptRect.setSize(Vector2f(window.getSize().x, window.getSize().y));
            scriptRect.setFillColor(Color(50, 50, 50, 200));
            window.draw(scriptRect);
            window.draw(label);
            RectangleShape nodeRect;
            nodeRect.setFillColor(Color::Black);
            for(int i = 0; i < nodes.size(); i++)
            {
                nodeRect.setSize(Vector2f(nodeSizeX, centerOffset + 10 * nodes[i].inputCount));
                nodeRect.setPosition(pos + nodes[i].offset);
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
                if(i == 0)
                    label.setString("BallPos");

                label.setFillColor(Color::White);
                window.draw(label);
            }

            for(int i = 0; i < nodes.size(); i++)
            {
                            CircleShape nodeCircle(5);
            nodeCircle.setFillColor(Color(0, 0, 0));
            nodeCircle.setOrigin(Vector2f(5, 5));
            VertexArray line(PrimitiveType::LineStrip, 2);
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
customConstraintScript customConstraint;
void start()
{
    ifstream config("res/config.ini");
    int r;
    int g;
    int b;
    if(config.good())
    {
        config >> buffer >> maxThreads >>
                  buffer >> gm.subSteps >>
                  buffer >> gm.constraintStrength >>
                  buffer >> gm.springStrength >> buffer>>
                  buffer >> r >>
                  buffer >> g >>
                  buffer >> b;
        defaultColor = Color(r, g, b);
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
    camera.move(Vector2f(0,-500));
    if(!font.openFromFile("res/font.ttf"))
        cout <<"Error, could not load font.ttf\n";
    if(!texture.loadFromFile("res/spriteSheet.png"))
        cout<<"Error, could not load spriteSheet.png\n";
    customConstraint.init(font);
    cur.setFillColor(Color::Green);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(true);

    for(unsigned int i = 0; i<buttonCount; i++)
        buttons.emplace_back(RectangleShape(Vector2f(60,60)));

    for(auto& u: ui)
        u.setFillColor(defaultColor);
    for(auto& b:buttons)
        b.setFillColor(Color::White);
    for(unsigned int i =0; i<buttonCount; i++)
    {
        buttons[i].setTextureRect(IntRect({(i%3)*16,round((i/3))*16},{16,16}));
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
    ui[1].setPosition({0,0});
    ui[2].setPosition({0,0});
}
int main()
{
    start();
    window.setFramerateLimit(60);
    while (window.isOpen())
    {
        while(const std::optional e = window.pollEvent())
        {


            customConstraint.updateUI(e, window);
            if(e->is<Event::Closed>())
                window.close();
            if(e->is<Event::MouseMoved>())
            {
                if(isMovingCamera && mode == 8)
                {
                    Vector2f delta = window.mapPixelToCoords(Vector2i(Mouse::getPosition(window).x,Mouse::getPosition(window).y)) - mousePos;
                    camera.move(delta*-cameraSpeed);
                }
                Vector2f newPos = window.mapPixelToCoords(Mouse::getPosition(window));
                mouseDelta = Vector2f(newPos - mousePos);
                mousePos = newPos;

            }
            if(e->is<Event::KeyReleased>())
            {
                int selected = getSelectedBall();
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::F11)
                {

                }
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::Delete)
                {
                    for(unsigned int i = 0; i < gm.rectAmount; i++)
                        if(UIselection(gm.rects[i]))
                        {
                            gm.deleteRect(i);
                        }
                    if(selected>-1)
                        gm.removeBall(selected);
                    for(unsigned int s = 0; s < multSelection.size(); s++)
                        gm.removeBall(multSelection[s]);
                    multSelection.clear();
                    customConstraint.unLink();
                }
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::R && mode == 10)
                    gm.createRect(selectionRect.getPosition(), selectionRect.getSize());
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::Up)
                {
                    switch(mode)
                    {
                    case(9):
                        clothHeight++;
                        break;
                    }
                }
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::Down)
                {
                    switch(mode)
                    {
                    case(9):
                        clothHeight--;
                        break;
                    }
                }
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::Left)
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
                if(e->getIf<Event::KeyReleased>() -> code == Keyboard::Key::Right)
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
            if(e->is<Event::MouseWheelScrolled>())
            {
                int scroll = e->getIf<Event::MouseWheelScrolled>() -> delta;
                if(mode == 8)
                {
                    scroll *= 100;
                    float ratioy = camera.getSize().y/camera.getSize().x;
                    camera.setSize(Vector2f(camera.getSize().x-scroll,camera.getSize().y - scroll*ratioy));
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
            if(e->is<Event::MouseButtonPressed>())
            {
                if(e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Left)
                {
                    isHolding = true;
                    onLeftClick();
                }
                else if(e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Middle)
                    isMovingCamera = true;
                else if(e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Right)
                {
                    onRightClick();
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
                    buttons[4].setTextureRect(IntRect({shouldBeStatic? 32:16,shouldBeStatic? yPos:16},{16,16}));
                }
                else if(UIselection(buttons[6]))
                    shouldShow = !shouldShow;
                else if(UIselection(buttons[11]))
                {
                    isExplosionReversed = !isExplosionReversed;
                    buttons[11].setTextureRect(IntRect({isExplosionReversed? 16:32, isExplosionReversed? yPos:48},{16,16}));
                }
                else if(UIselection(buttons[13]))
                {
                    int bAmount = gm.ballAmount;
                    for(unsigned int i = 0; i < bAmount; i++)
                    {
                        gm.removeBall(i);
                        gm.ballAmount--;
                    }
                    for(unsigned int i = 0; i < gm.rectAmount; i++)
                        gm.deleteRect(i);
                    multSelection.clear();
                }
                else if(UIselection(buttons[15]) && multSelection.size() > 0)
                    saveContraption();
                else if(UIselection(buttons[16]))
                    showScript = !showScript;
            }
            if(e->is<Event::MouseButtonReleased>())
            {
                if(e->getIf<Event::MouseButtonReleased>() -> button == Mouse::Button::Left)
                {
                    targetIndex = -1;
                    firstBall= -1;
                    currentChain = 0;
                    isHolding= false;
                }
                else if(e->getIf<Event::MouseButtonReleased>() -> button == Mouse::Button::Middle)
                    isMovingCamera = false;
            }
            if(e->is<Event::KeyPressed>() && e->getIf<Event::KeyPressed>() -> code == Keyboard::Key::Space)
                isPaused = !isPaused;
        }
        CircleShape preview;
        Text paramText(font, "", 30);
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
        {
            gm.applyConstraints(maxThreads);
            for(int i = 0; i < gm.ballAmount; i++)
            {
                physicsEngine::ball& b = gm.balls[i];
                for(int c = 0; c < b.anchorCount; c++)
                {
                    if(b.constraintMode[c] == "Custom")
                    {

                        //b.sprite.setPosition(customConstraint.nodes[0].getOutput(customConstraint.nodes, b.sprite.getPosition(), gm.balls[b.anchorPointsIndex[c]].sprite.getPosition()));
                        b.sprite.setPosition(customConstraint.getPos(b.sprite.getPosition(), gm.balls[b.anchorPointsIndex[c]].sprite.getPosition()));
                    }
                }
            }
        }
        window.clear(Color::White);
        window.setTitle("Physics Playground FPS:" + gm.toString(fps()));
        window.setView(camera);
        applyUIAnchors();

        window.draw(paramText);
        for(unsigned int r = 0; r < gm.rectAmount; r++)
            window.draw(gm.rects[r]);
        for(unsigned int i =0; i<gm.ballAmount; i++)
        {
            if(gm.balls[i].shouldShowStats)
                gm.balls[i].showStats(window,mousePos,font);
            if(!isPaused)
            {
                gm.balls[i].acc.y +=98.8;
                gm.balls[i].upDatePos((float)1/60);
            }
            if(gm.balls[i].sprite.getPosition().y >= ui[1].getPosition().y - gm.balls[i].sprite.getRadius())
            {
                gm.balls[i].sprite.setPosition(Vector2f(gm.balls[i].sprite.getPosition().x,ui[1].getPosition().y - gm.balls[i].sprite.getRadius()));
                gm.balls[i].updateFriction();
            }
            else
            {
                gm.balls[i].acc.x = 0;
            }
            VertexArray line(PrimitiveType::LineStrip, 2);
            for(unsigned int c =0; c<gm.balls[i].anchorCount; c++)
            {

                line[0].color = defaultColor;
                line[1].color = defaultColor;
                line[0].position = gm.balls[i].sprite.getPosition();
                line[1].position = gm.balls[gm.balls[i].anchorPointsIndex[c]].sprite.getPosition();
                if(gm.balls[i].constraintMode[c] != "Custom" || customConstraint.showLine)
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
                preview.setPosition(Vector2f(mousePos.x,mousePos.y));
                preview.setOrigin(Vector2f(rad,rad));
                preview.setFillColor(Color(50,50,50,50));
                window.draw(preview);
                break;
            case(11):
                preview.setRadius(explosionRad);
                preview.setPosition(Vector2f(mousePos.x, mousePos.y));
                preview.setOrigin(Vector2f(explosionRad,explosionRad));
                preview.setFillColor(Color(255,0,0,50));
                window.draw(preview);
                break;
            case(14):
                loadContraption(mousePos, true);
                break;
        }
        if(mode == 10 && isHolding)
            window.draw(selectionRect);
        int selected = getSelectedBall();
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
            customConstraint.showScriptVisual(window, font, ui[0].getPosition() + Vector2f(ui[0].getSize().x, 0), Vector2f(400, 400));
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
        if(getSelectedBall() ==-1) return;
        else firstElement = getSelectedBall();
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
    for(unsigned int i = 0; i<gm.ballAmount; i++)
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
        for(unsigned int i =0; i<gm.ballAmount; i++)
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
        gm.balls[targetIndex].sprite.move(Vector2f((mousePos.x-gm.balls[targetIndex].sprite.getPosition().x)/2, (mousePos.y-gm.balls[targetIndex].sprite.getPosition().y)/2));
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
    ui[1].setPosition(Vector2f(window.mapPixelToCoords(Vector2i(0,1080)).x,0));
    ui[0].setSize(Vector2f(camera.getSize().x/(940/200),camera.getSize().y));
    ui[1].setSize(Vector2f(camera.getSize().x, camera.getSize().y));
    //paramText.setPosition(Vector2f(window.mapPixelToCoords(Vector2i((940),paramText.getCharacterSize()))));
    //paramText.setScale(Vector2f(camera.getSize().x / 940, camera.getSize().y/540));
    Vector2f ratioSize = Vector2f(940/60,540/60);

    for(unsigned int i = 0; i<buttons.size(); i++)
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
        selectionRect.setPosition(Vector2f(mousePos.x, mousePos.y));
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
    selectionCircle.setOrigin(Vector2f(ballRadius,ballRadius));
    selectionCircle.setPosition(gm.balls[index].sprite.getPosition());
    window.draw(selectionCircle);
    selectionCircle.setRadius(ballRadius-3);
    selectionCircle.setFillColor(defaultColor);
    selectionCircle.setOrigin(Vector2f(ballRadius-3,ballRadius-3));
    window.draw(selectionCircle);
}
void saveContraption()
{
    Vector2f origin = Vector2f(gm.balls[multSelection[0]].sprite.getPosition());
    map<int, int> ballsTransformed;
    for(unsigned int i = 0; i < multSelection.size(); i++)
    {
        ballsTransformed.insert(make_pair(multSelection[i],i));
    }
    ofstream file("contraption" + gm.toString(contraptionIndex) + ".contr");
    file << multSelection.size() << endl;
    for(unsigned int i = 0; i < multSelection.size(); i++)
    {
        physicsEngine::ball b = gm.balls[multSelection[i]];
        file << b.sprite.getPosition().x - origin.x << endl
             << b.sprite.getPosition().y - origin.y << endl
             << b.sprite.getRadius() << endl
             << b.isStatic << endl
             << b.friction << endl
             << b.rotationSpeed << endl
             << gm.balls[multSelection[i]].anchorCount << endl;
        for(unsigned int j = 0; j < gm.balls[multSelection[i]].anchorCount; j++)
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
    for(unsigned int i = 0; i < ballCount; i++)
    {
        file >> pos.x >> pos.y >> radius >> isStatic >> friction >> rotationSpeed >> anchorCount;
        if(isPreview)
        {
            CircleShape preview(radius);
            preview.setPosition(Vector2f(pos+offset));
            preview.setOrigin(Vector2f(radius,radius));
            preview.setFillColor(Color(0,0,0,50));
            window.draw(preview);
            for(unsigned int j = 0; j < anchorCount; j++)
            {
                file >> anchorIndex >> maxDist >> cMode;
            }
        }
        else
        {
            gm.createBall(Vector2f(pos+offset),isStatic == 1 ? true : false, true);
            gm.balls[gm.ballAmount-1].sprite.setRadius(radius);
            gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(radius,radius));
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].friction = friction;
            gm.balls[gm.ballAmount-1].rotationSpeed = rotationSpeed;
            for(unsigned int j = 0; j < anchorCount; j++)
            {
                bool shouldAddConstraint = true;
                file >> anchorIndex >> maxDist >> cMode;
                for(unsigned int c = 0; c < gm.balls[gm.ballAmount-1].anchorCount; c++)
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
void onRightClick()
{
    switch(mode)
    {
        case(16):
            customConstraint.removeNode();
            break;

    }
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
            for(unsigned int i = 0; i<clothHeight; i++)
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
                for(unsigned int j = 0; j<clothHeight; j++)
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
        customConstraint.grab(mousePos);
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
            for(unsigned int b=0; b<gm.ballAmount; b++)
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
            gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(5,5));
            gm.balls[gm.ballAmount-1].sprite.setFillColor(defaultColor);
            gm.balls[gm.ballAmount-1].rotationSpeed = motorSpeed;
            double ratioAngle = (double)360/points;
            for(unsigned int i = 0; i<points; i++)
            {
                gm.createBall(origin, false, true);
                gm.balls[gm.ballAmount-1].sprite.setPosition(Vector2f(origin.x+sin(gm.ballAmount*ratioAngle*rad) * 50, origin.y+cos(gm.ballAmount*ratioAngle*rad) * 50));
                gm.balls[gm.ballAmount-1].sprite.setRadius(5);
                gm.balls[gm.ballAmount-1].sprite.setOrigin(Vector2f(5,5));
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
        case(16):
            customConstraint.link();
            break;
        }
        if(getSelectedBall() == -1)
        {
            selectionOriginDefined=false;
            multSelection.clear();
        }
    }
}
