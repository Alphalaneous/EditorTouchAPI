#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenu.hpp>

using namespace geode::prelude;

class $modify(TouchCCMenu, CCMenu) {
    bool initWithArray(CCArray* pArrayOfItems);

    CCMenuItem* itemForTouch_(CCTouch* touch, bool);
};