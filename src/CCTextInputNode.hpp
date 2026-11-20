#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

using namespace geode::prelude;

class $modify(TouchCCTextInputNode, CCTextInputNode) {

    bool init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont);

};