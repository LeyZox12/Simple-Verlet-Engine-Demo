#include "UIutils.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <functional>

using namespace sf;
using namespace std;


UIutils::UIutils()
{

}
void UIutils::addButton(Vector2f pos, Vector2f sizeRect, function<void()> command, string name)
{
    button newButton;
    newButton.pos = pos;
    newButton.buttonSize = sizeRect;
    newButton.buttonRect.setPosition(pos);
    newButton.buttonRect.setSize(sizeRect);
    newButton.command = command;
    newButton.buttonNameStr = name;
    buttons.push_back(newButton);
}
void UIutils::addDropDown(Vector2f pos, Vector2f sizeRect, vector<string> elements, int maxElementPage, string baseText)
{
    dropDown newDropDown;
    newDropDown.pos = pos;
    newDropDown.sizeRect = sizeRect;
    newDropDown.dropRect.setPosition(pos);
    newDropDown.dropRect.setSize(sizeRect);
    newDropDown.value = baseText;
    for(auto& s : elements)
        newDropDown.addElement(s);
    newDropDown.maxPageElements = maxElementPage;
    dropDowns.push_back(newDropDown);
}
void UIutils::addSlider(Vector2f pos, Vector2f sizeRect, float maxVal, bool hasKnob, bool isValueFixed)
{
    slider newSlider;
    newSlider.pos = pos;
    newSlider.sizeRect = sizeRect;
    newSlider.backgroundRect.setPosition(pos);
    newSlider.backgroundRect.setSize(sizeRect);
    newSlider.maxValue = maxVal;
    newSlider.isValueFixed = isValueFixed;
    newSlider.hasknob = hasKnob;
    sliders.push_back(newSlider);
}
void UIutils::addTextZone(Vector2f pos, Vector2f sizeRect, int txtSize)
{
    textZone newTextZone;
    newTextZone.pos = pos;
    newTextZone.sizeRect = sizeRect;
    newTextZone.textZoneRect.setPosition(pos);
    newTextZone.textZoneRect.setSize(sizeRect);
    newTextZone.txtSize = txtSize;
    textZones.push_back(newTextZone);
}
void UIutils::updateElements(optional<Event> e, RenderWindow &window)
{

    if(e->is<Event::MouseButtonPressed>() && e->getIf<Event::MouseButtonPressed>() -> button == Mouse::Button::Left)
    {
        Vector2i mousepos = Vector2i(window.mapPixelToCoords(Mouse::getPosition(window)));
        for(int i = 0; i < buttons.size(); i++)
            buttons[i].onClick(mousepos);
        for(int i = 0; i < dropDowns.size(); i++)
            dropDowns[i].getSelected(mousepos, true);
        for(int i = 0; i < textZones.size(); i++)
            textZones[i].updateShouldWrite(mousepos);
        for(int i = 0; i < sliders.size(); i++)
            sliders[i].updateValue(mousepos);
    }
    else if(e->is<Event::TextEntered>())
    {
        for(int i = 0; i < textZones.size(); i++)
        {
            int unicodeVal = e->getIf<Event::TextEntered>() -> unicode;
            if(unicodeVal < 128)
            {
                textZones[i].write(unicodeVal);
            }
        }
    }
    else if(e->is<Event::MouseWheelScrolled>())
    {
        Vector2i mousepos = Vector2i(window.mapPixelToCoords(Mouse::getPosition(window)));
        for(int i = 0; i < dropDowns.size(); i++)
            dropDowns[i].scroll(e->getIf<Event::MouseWheelScrolled>() -> delta, mousepos);
    }
}

void UIutils::addButton(Vector2f pos, Vector2f sizeRect, function<void()> command, string name, Color buttonColor, Color selectedColor, Color textColor)
{
    button newButton;
    newButton.pos = pos;
    newButton.buttonSize = sizeRect;
    newButton.buttonRect.setPosition(pos);
    newButton.buttonRect.setSize(sizeRect);
    newButton.command = command;
    newButton.buttonNameStr = name;
    newButton.buttonColor = buttonColor;
    newButton.selectedColor = selectedColor;
    newButton.textColor = textColor;
    buttons.push_back(newButton);
}
void UIutils::addDropDown(Vector2f pos, Vector2f sizeRect, vector<string> elements, int maxElementPage, string baseText, Color baseColor, Color elementColor, Color textColor, Color selectedColor)
{
    dropDown newDropDown;
    newDropDown.pos = pos;
    newDropDown.sizeRect = sizeRect;
    newDropDown.dropRect.setPosition(pos);
    newDropDown.dropRect.setSize(sizeRect);
    newDropDown.value = baseText;
    newDropDown.elementsColor = elementColor;
    newDropDown.rectColor = baseColor;
    newDropDown.textColor = textColor;
    newDropDown.selectedColor = selectedColor;
    for(auto& s : elements)
        newDropDown.addElement(s);
    newDropDown.maxPageElements = maxElementPage;
    dropDowns.push_back(newDropDown);
}
void UIutils::addSlider(Vector2f pos, Vector2f sizeRect, float maxVal, bool hasKnob, bool isValueFixed, Color backgroundColor, Color fillColor, Color textColor)
{
    slider newSlider;
    newSlider.pos = pos;
    newSlider.sizeRect = sizeRect;
    newSlider.backgroundRect.setPosition(pos);
    newSlider.backgroundRect.setSize(sizeRect);
    newSlider.maxValue = maxVal;
    newSlider.hasknob = hasKnob;
    newSlider.isValueFixed = isValueFixed;
    newSlider.backgroundColor = backgroundColor;
    newSlider.fillColor = fillColor;
    newSlider.textColor = textColor;
    sliders.push_back(newSlider);
}
void UIutils::addTextZone(Vector2f pos, Vector2f sizeRect, int txtSize, Color rectColor, Color textColor)
{
    textZone newTextZone;
    newTextZone.pos = pos;
    newTextZone.sizeRect = sizeRect;
    newTextZone.textZoneRect.setPosition(pos);
    newTextZone.textZoneRect.setSize(sizeRect);
    newTextZone.txtSize = txtSize;
    newTextZone.rectColor = rectColor;
    newTextZone.textColor = textColor;
    textZones.push_back(newTextZone);
}
void UIutils::displayElements(RenderWindow &window)
{
    for(int i = 0; i < buttons.size(); i++)
        buttons[i].display(window, font);
    for(int i = 0; i < dropDowns.size(); i++)
        dropDowns[i].display(window, font);
    for(int i = 0; i < textZones.size(); i++)
        textZones[i].display(window, font);
    for(int i = 0; i < sliders.size(); i++)
        sliders[i].display(window, font);
}
