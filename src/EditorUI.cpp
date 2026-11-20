#include "EditorUI.hpp"
#include "TouchHandler.hpp"
#include "../include/EditorTouch.hpp"
#include "../include/API.hpp"

using namespace alpha::editor_touch;

TouchEditorUI* TouchEditorUI::s_instance = nullptr;

TouchEditorUI* TouchEditorUI::get() {
    return s_instance;
}

bool TouchEditorUI::init(LevelEditorLayer* editorLayer) {
    s_instance = this;
    if (!EditorUI::init(editorLayer)) return false;

    m_fields->m_touchHandler = TouchHandler::create(this);
    setupEvents();

    addEventListener(TouchBegin(), [this] (TouchAction& ret, alpha::dispatcher::Touch* touch) {
        if (touch->getButton() != geode::MouseInputData::Button::Left) return;
        ret = ccTouchBegan(touch->getOriginalTouch(), nullptr);
    });

    addEventListener(TouchMoved(), [this] (alpha::dispatcher::Touch* touch) {
        if (touch->getButton() != geode::MouseInputData::Button::Left) return;
        ccTouchMoved(touch->getOriginalTouch(), nullptr);
    });

    addEventListener(TouchEnded(), [this] (alpha::dispatcher::Touch* touch) {
        if (touch->getButton() != geode::MouseInputData::Button::Left) return;
        ccTouchEnded(touch->getOriginalTouch(), nullptr);
    });

    addEventListener(TouchCancelled(), [this] (alpha::dispatcher::Touch* touch) {
        if (touch->getButton() != geode::MouseInputData::Button::Left) return;
        ccTouchCancelled(touch->getOriginalTouch(), nullptr);
    });

    return true;
}

void TouchEditorUI::draw() {
    auto oldStart = m_swipeStart;
    auto oldEnd = m_swipeEnd;

    m_swipeStart = TouchHandler::rotatePoint(m_swipeStart, true);
    m_swipeEnd = TouchHandler::rotatePoint(m_swipeEnd, true);

    EditorUI::draw();

    m_swipeStart = oldStart;
    m_swipeEnd = oldEnd;
}

bool TouchEditorUI::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        m_editorLayer->m_uiLayer->ccTouchBegan(touch, nullptr);
    }

    if (handleTouchControlsBegin(touch)) {
        return true;
    }

    if (beginPinchGesture(touch)) {
        return true;
    }

    return beginEditorTouch(touch);
}

void TouchEditorUI::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    if (handleTouchControlsMove(touch)) return;

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        m_editorLayer->m_uiLayer->ccTouchMoved(touch, event);
        return;
    }

    moveEditorTouch(touch);
}

void TouchEditorUI::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    m_canActivateControls = false;
    stopSwipeTrigger(touch);

    if (handleTouchControlsEnd(touch)) {
        return;
    }

    int id = touch->getID();

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        m_editorLayer->m_uiLayer->ccTouchEnded(touch, event);
        return;
    }

    if (m_unk31c) {
        endPinchGesture(touch);
        return;
    }

    if (m_touchID != id) return;

    endEditorTouch(touch);
}