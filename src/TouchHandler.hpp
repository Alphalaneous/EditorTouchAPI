#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>

using namespace geode::prelude;

namespace alpha::editor_touch {

class RotateTouch {
public:
    RotateTouch(cocos2d::CCTouch* touch);
    ~RotateTouch();
protected:
    cocos2d::CCTouch* m_touch = nullptr;

    cocos2d::CCPoint m_startPoint{};
    cocos2d::CCPoint m_point{};
    cocos2d::CCPoint m_prevPoint{};
};

class TouchHandler : public CCNode, public alpha::dispatcher::TouchDelegate {
public:
    static TouchHandler* create(EditorUI* editorUI);
    static TouchHandler* get();

    void onEnter() override;
    void onExit() override;

    bool canTouch(CCTouch* touch);
    bool overItem(CCTouch* touch);

    static cocos2d::CCPoint rotatePoint(const cocos2d::CCPoint& point, bool inverse = false);

    bool touchBegan(alpha::dispatcher::Touch* touch) override;
	void touchMoved(alpha::dispatcher::Touch* touch) override;
	void touchEnded(alpha::dispatcher::Touch* touch) override;
	void touchCancelled(alpha::dispatcher::Touch* touch) override;

    void addMenu(CCMenu* menu);
    void removeMenu(CCMenu* menu);

    void addCCTextInputNode(CCTextInputNode* node);
    void removeCCTextInputNode(CCTextInputNode* node);

protected:
    bool init(EditorUI* editorUI);

    EditorUI* m_editorUI;
    std::unordered_set<CCMenu*> m_menus;
    std::unordered_set<CCTextInputNode*> m_inputNodes;
};

};