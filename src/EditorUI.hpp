#pragma once

#include "TouchHandler.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <asp/time/SystemTime.hpp>

using namespace geode::prelude;

class $modify(TouchEditorUI, EditorUI) {
    static TouchEditorUI* s_instance;

    struct Fields {
        Ref<alpha::editor_touch::TouchHandler> m_touchHandler;
        ~Fields() {
            s_instance = nullptr;
        }
    };

    static TouchEditorUI* get();

    bool init(LevelEditorLayer* editorLayer);
    void draw();
    void setupEvents();

    bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);
    void ccTouchEnded(CCTouch* touch, CCEvent* event);
};