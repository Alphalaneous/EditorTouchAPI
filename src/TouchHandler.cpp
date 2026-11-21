#include "TouchHandler.hpp"
#include "CCMenu.hpp"
#include "EditorUI.hpp"
#include <alphalaneous.alphas-ui-pack/Utils.hpp>
#include "../include/EditorTouch.hpp"

namespace alpha::editor_touch {

RotateTouch::RotateTouch(CCTouch* touch) {
    if (!touch) return;

    m_touch = touch;
    m_startPoint = touch->m_startPoint;
    m_point = touch->m_point;
    m_prevPoint = touch->m_prevPoint;

    auto winSize = CCDirector::get()->getWinSize();
    auto editorLayer = LevelEditorLayer::get();

    auto newPoint = TouchHandler::rotatePoint(touch->getLocation());
    auto newStart = TouchHandler::rotatePoint(touch->getStartLocation());
    auto newPrev = TouchHandler::rotatePoint(touch->getPreviousLocation());
    
    touch->m_point = CCPoint{newPoint.x, winSize.height - newPoint.y};
    touch->m_startPoint = CCPoint{newStart.x, winSize.height - newStart.y};
    touch->m_prevPoint = CCPoint{newPrev.x, winSize.height - newPrev.y};
}

RotateTouch::~RotateTouch() {
    if (!m_touch) return;

    m_touch->m_startPoint = m_startPoint;
    m_touch->m_point = m_point;
    m_touch->m_prevPoint = m_prevPoint;
}

TouchHandler* TouchHandler::get() {
    auto editorUI = TouchEditorUI::get();
    if (!editorUI) return nullptr;
    
    return editorUI->m_fields->m_touchHandler;
}

TouchHandler* TouchHandler::create(EditorUI* editorUI) {
    auto ret = new TouchHandler();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

cocos2d::CCPoint TouchHandler::rotatePoint(const cocos2d::CCPoint& point, bool inverse) {
    CCPoint pivot = CCDirector::get()->getWinSize() / 2.f;
    auto editorLayer = LevelEditorLayer::get();

    auto rot = editorLayer->m_gameState.m_cameraAngle * (inverse ? -1 : 1);
    float angleRadians = CC_DEGREES_TO_RADIANS(rot);

    float sinA = std::sinf(angleRadians);
    float cosA = std::cosf(angleRadians);

    auto newPoint = point - pivot;

    float xNew = newPoint.x * cosA - newPoint.y * sinA;
    float yNew = newPoint.x * sinA + newPoint.y * cosA;

    return cocos2d::CCPoint(xNew + pivot.x, yNew + pivot.y);
}

bool TouchHandler::init(EditorUI* editorUI) {
    m_editorUI = editorUI;
    editorUI->addOnEnterCallback([this] {
        CCTouchDispatcher::get()->removeDelegate(m_editorUI);
        m_editorUI->m_editorLayer->addChild(this);
        setZOrder(m_editorUI->getZOrder() - 1);
    });
    editorUI->addOnExitCallback([this] {
        m_editorUI->m_editorLayer->removeChild(this);
    });

    return true;
}

void TouchHandler::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

void TouchHandler::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

bool TouchHandler::canTouch(CCTouch* touch) {
    if (m_editorUI->m_editorLayer->m_playbackMode == PlaybackMode::Playing && !m_editorUI->m_playbackBtn->isVisible()) {
        return true;
    }

    if (touch->getLocation().y < m_editorUI->m_toolbarHeight && !m_editorUI->m_uiHidden) {
        return false;
    }

    return true;
}

void TouchHandler::addMenu(CCMenu* menu) {
    m_menus.insert(menu);
}

void TouchHandler::removeMenu(CCMenu* menu) {
    m_menus.erase(menu);
}

void TouchHandler::addCCTextInputNode(CCTextInputNode* node) {
    m_inputNodes.insert(node);
}

void TouchHandler::removeCCTextInputNode(CCTextInputNode* node) {
    m_inputNodes.erase(node);
}

bool TouchHandler::overItem(CCTouch* touch) {
    if (m_editorUI->m_isPaused) return true;

    for (auto child : CCScene::get()->getChildrenExt()) {
        auto alert = typeinfo_cast<FLAlertLayer*>(child);
        if (!alert) continue;

        if (auto window = typeinfo_cast<EditorWindowPopup*>(alert)) {
            bool inside = utils::isPointInsideNode(window->m_background, touch->getLocation());
            if (inside) return true;
            continue;
        }
        return true;
    }

    for (auto child : m_editorUI->getChildrenExt()) {
        auto alert = typeinfo_cast<FLAlertLayer*>(child);
        if (!alert) continue;

        auto bg = alert->m_mainLayer->getChildByType<CCScale9Sprite>();
        if (!bg) continue;

        bool inside = utils::isPointInsideNode(bg, touch->getLocation());
        if (inside) return true;
    }

    if (m_editorUI->m_hsvOverlay && m_editorUI->m_hsvOverlay->getParent() && m_editorUI->m_hsvOverlay->isVisible()) {
        auto bg = m_editorUI->m_hsvOverlay->m_widget->getChildByID("background");
        if (bg) {
            bool inside = utils::isPointInsideNode(bg, touch->getLocation());
            if (inside) return true;
        }
    }

    for (auto menu : m_menus) {
        if (!nodeIsVisible(menu)) continue;

        auto item = static_cast<TouchCCMenu*>(menu)->itemForTouch_(touch, false);
        if (item) return true;
    }
    for (auto node : m_inputNodes) {
        if (!nodeIsVisible(node)) continue;

        auto ignore = node->isIgnoreAnchorPointForPosition();
        node->ignoreAnchorPointForPosition(false);
        bool inside = utils::isPointInsideNode(node, touch->getLocation());
        node->ignoreAnchorPointForPosition(ignore);

        if (inside) return true;
    }
    return false;
}

bool TouchHandler::touchBegan(alpha::dispatcher::Touch* touch) {
    if (!canTouch(touch)) {
        return true;
    }

    if (overItem(touch) && touch->getButton() != geode::MouseInputData::Button::Left) return false;

    auto rotated = RotateTouch(touch);
    auto rotatedOriginal = RotateTouch(touch->getOriginalTouch());

    TouchAction ret = false;
    TouchBegin().send(ret, touch);
    return ret;
}

void TouchHandler::touchMoved(alpha::dispatcher::Touch* touch) {
    auto rotated = RotateTouch(touch);
    auto rotatedOriginal = RotateTouch(touch->getOriginalTouch());

    TouchMoved().send(touch);
}

void TouchHandler::touchEnded(alpha::dispatcher::Touch* touch) {
    auto rotated = RotateTouch(touch);
    auto rotatedOriginal = RotateTouch(touch->getOriginalTouch());

    TouchEnded().send(touch);
}

void TouchHandler::touchCancelled(alpha::dispatcher::Touch* touch) {
    auto rotated = RotateTouch(touch);
    auto rotatedOriginal = RotateTouch(touch->getOriginalTouch());

    TouchCancelled().send(touch);
}

}