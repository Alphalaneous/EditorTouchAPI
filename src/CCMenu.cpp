#include "CCMenu.hpp"
#include "EditorUI.hpp"
#include "TouchHandler.hpp"

bool TouchCCMenu::initWithArray(CCArray* pArrayOfItems) {
    if (!CCMenu::initWithArray(pArrayOfItems)) return false;
    if (!TouchEditorUI::get()) return true;

    addOnEnterCallback([this] {
        auto handler = alpha::editor_touch::TouchHandler::get();
        if (!handler) return;
        handler->addMenu(this);
    });

    addOnExitCallback([this] {
        auto handler = alpha::editor_touch::TouchHandler::get();
        if (!handler) return;
        handler->removeMenu(this);
    });

    return true;
}

CCMenuItem* TouchCCMenu::itemForTouch_(CCTouch* touch, bool p1) {
    return CCMenu::itemForTouch(touch, p1);
}