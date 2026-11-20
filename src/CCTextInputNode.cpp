#include "CCTextInputNode.hpp"
#include "EditorUI.hpp"
#include "TouchHandler.hpp"

bool TouchCCTextInputNode::init(float width, float height, char const* placeholder, char const* textFont, int fontSize, char const* labelFont) {
    if (!CCTextInputNode::init(width, height, placeholder, textFont, fontSize, labelFont)) return false;
    if (!TouchEditorUI::get()) return true;

    addOnEnterCallback([this] {
        auto handler = alpha::editor_touch::TouchHandler::get();
        if (!handler) return;
        handler->addCCTextInputNode(this);
    });

    addOnExitCallback([this] {
        auto handler = alpha::editor_touch::TouchHandler::get();
        if (!handler) return;
        handler->removeCCTextInputNode(this);
    });

    return true;
}