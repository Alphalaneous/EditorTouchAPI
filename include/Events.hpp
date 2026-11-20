#pragma once

#include <Geode/cocos/touch_dispatcher/CCTouch.h>
#include <Geode/loader/Event.hpp>

/*
 * Everything here is how the Editor Touch works but separated into different events. This was done to allow
 * for devs to target specific actions. Since events can be set to Propagate or Stop and have a priority you can
 * set, you can override these methods to do what you want, somewhat like hooking them. I recommend looking at
 * this mod's source code to see the specifics of each one if you wish to make changes to them. Be sure to handle
 * the ret params correctly as you cannot return a value in events normally. The other arguments can be modified too
 * as they are passed as reference. So if you wish to change those, you can, just be sure not to accidentally change
 * them if you just want the change locally.
 */
 
namespace alpha::editor_touch::events {

class SnapToGrid : public geode::Event<SnapToGrid, bool(cocos2d::CCPoint& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};
    
class StopSwipeTrigger : public geode::Event<StopSwipeTrigger, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class CanFreeMove : public geode::Event<CanFreeMove, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class RefreshSelectionUI : public geode::Event<RefreshSelectionUI, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class GetSelectableObject : public geode::Event<GetSelectableObject, bool(GameObject*& ret, cocos2d::CCTouch*& touch, cocos2d::CCArray*& objects)> {
public:
    using Event::Event;
};

class SelectPickedObject : public geode::Event<SelectPickedObject, bool(cocos2d::CCTouch*& touch, GameObject*& object)> {
public:
    using Event::Event;
};

class DeselectLastObject : public geode::Event<DeselectLastObject, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class FindSnapObjectAtPos : public geode::Event<FindSnapObjectAtPos, bool(GameObject*& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class InitializeSnapObject : public geode::Event<InitializeSnapObject, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class CreateFreeMoveUndo : public geode::Event<CreateFreeMoveUndo, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class BeginFreeMove : public geode::Event<BeginFreeMove, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class HandleTouchControlsBegin : public geode::Event<HandleTouchControlsBegin, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class HandleTouchControlsMove : public geode::Event<HandleTouchControlsMove, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class HandleTouchControlsEnd : public geode::Event<HandleTouchControlsEnd, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class BeginPinchGesture : public geode::Event<BeginPinchGesture, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class MovePinchGesture : public geode::Event<MovePinchGesture, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class EndPinchGesture : public geode::Event<EndPinchGesture, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class SwipeTouchMoved : public geode::Event<SwipeTouchMoved, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class SelectSwipeRectangle : public geode::Event<SelectSwipeRectangle, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class SelectSwipeTap : public geode::Event<SelectSwipeTap, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class HandleSwipeSelection : public geode::Event<HandleSwipeSelection, bool(cocos2d::CCTouch*& touch, bool& tapped)> {
public:
    using Event::Event;
};

class MoveSelectionBy : public geode::Event<MoveSelectionBy, bool(cocos2d::CCTouch*& touch, cocos2d::CCPoint& offset)> {
public:
    using Event::Event;
};

class MoveTransformControls : public geode::Event<MoveTransformControls, bool(cocos2d::CCTouch*& touch, cocos2d::CCPoint& offset)> {
public:
    using Event::Event;
};

class ShouldContinueSwipe : public geode::Event<ShouldContinueSwipe, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class MoveFreeMoveObject : public geode::Event<MoveFreeMoveObject, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class MoveCamera : public geode::Event<MoveCamera, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class SnapFreeMoveObject : public geode::Event<SnapFreeMoveObject, bool(bool& ret, cocos2d::CCTouch*& touch, cocos2d::CCPoint& offset)> {
public:
    using Event::Event;
};

class FinishFreeMove : public geode::Event<FinishFreeMove, bool(cocos2d::CCTouch*& touch, bool& dropSnapped)> {
public:
    using Event::Event;
};

class CommitFreeMoveUndo : public geode::Event<CommitFreeMoveUndo, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class BeginEditorTouch : public geode::Event<BeginEditorTouch, bool(bool& ret, cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class MoveEditorTouch : public geode::Event<MoveEditorTouch, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class EndEditorTouch : public geode::Event<EndEditorTouch, bool(cocos2d::CCTouch*& touch)> {
public:
    using Event::Event;
};

class HandleTap : public geode::Event<HandleTap, bool(cocos2d::CCTouch*& touch, bool& tapped, bool& doubleTapped, float& distance)> {
public:
    using Event::Event;
};

class ResetTouchState : public geode::Event<ResetTouchState, bool(cocos2d::CCTouch*& touch, bool& activateControls)> {
public:
    using Event::Event;
};

class IsRightAngle : public geode::Event<IsRightAngle, bool(bool& ret, cocos2d::CCTouch*& touch, float& rotation)> {
public:
    using Event::Event;
};

}