#define GEODE_DEFINE_EVENT_EXPORTS
#include "../include/API.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace alpha::editor_touch {

bool isRightAngle(CCTouch* touch, float rotation) {
    bool ret = false;
    events::IsRightAngle().send(ret, touch, rotation);
    return ret;
}

CCPoint snapToGrid(CCTouch* touch) {
    CCPoint ret{};
    events::SnapToGrid().send(ret, touch);
    return ret;
}

void stopSwipeTrigger(CCTouch* touch) {
    events::StopSwipeTrigger().send(touch);
}

bool canFreeMove(CCTouch* touch) {
    bool ret = false;
    events::CanFreeMove().send(ret, touch);
    return ret;
}

void refreshSelectionUI(CCTouch* touch) {
    events::RefreshSelectionUI().send(touch);
}

GameObject* getSelectableObject(CCTouch* touch, CCArray* objects) {
    GameObject* ret = nullptr;
    events::GetSelectableObject().send(ret, touch, objects);
    return ret;
}

void selectPickedObject(CCTouch* touch, GameObject* object) {
    events::SelectPickedObject().send(touch, object);
}

void deselectLastObject(CCTouch* touch) {
    events::DeselectLastObject().send(touch);
}

GameObject* findSnapObjectAtPos(CCTouch* touch) {
    GameObject* ret = nullptr;
    events::FindSnapObjectAtPos().send(ret, touch);
    return ret;
}

void initializeSnapObject(CCTouch* touch) {
    events::InitializeSnapObject().send(touch);
}

void createFreeMoveUndo(CCTouch* touch) {
    events::CreateFreeMoveUndo().send(touch);
}

void beginFreeMove(CCTouch* touch) {
    events::BeginFreeMove().send(touch);
}

bool handleTouchControlsBegin(CCTouch* touch) {
    bool ret = false;
    events::HandleTouchControlsBegin().send(ret, touch);
    return ret;
}

bool handleTouchControlsMove(CCTouch* touch) {
    bool ret = false;
    events::HandleTouchControlsMove().send(ret, touch);
    return ret;
}

bool handleTouchControlsEnd(CCTouch* touch) {
    bool ret = false;
    events::HandleTouchControlsEnd().send(ret, touch);
    return ret;
}

bool beginPinchGesture(CCTouch* touch) {
    bool ret = false;
    events::BeginPinchGesture().send(ret, touch);
    return ret;
}

void movePinchGesture(CCTouch* touch) {
    events::MovePinchGesture().send(touch);
}

void endPinchGesture(CCTouch* touch) {
    events::EndPinchGesture().send(touch);
}

bool swipeTouchMoved(CCTouch* touch) {
    bool ret = false;
    events::SwipeTouchMoved().send(ret, touch);
    return ret;
}

void selectSwipeRectangle(CCTouch* touch) {
    events::SelectSwipeRectangle().send(touch);
}

void selectSwipeTap(CCTouch* touch) {
    events::SelectSwipeTap().send(touch);
}

void handleSwipeSelection(CCTouch* touch, bool tapped) {
    events::HandleSwipeSelection().send(touch, tapped);
}

void moveSelectionBy(CCTouch* touch, CCPoint& offset) {
    events::MoveSelectionBy().send(touch, offset);
}

void moveTransformControls(CCTouch* touch, CCPoint& offset) {
    events::MoveTransformControls().send(touch, offset);
}

bool shouldContinueSwipe(CCTouch* touch) {
    bool ret = false;
    events::ShouldContinueSwipe().send(ret, touch);
    return ret;
}

void moveFreeMoveObject(CCTouch* touch) {
    events::MoveFreeMoveObject().send(touch);
}

void moveCamera(CCTouch* touch) {
    events::MoveCamera().send(touch);
}

bool snapDraggedObject(CCTouch* touch, CCPoint& offset) {
    bool ret = false;
    events::SnapFreeMoveObject().send(ret, touch, offset);
    return ret;
}

void finishFreeMove(CCTouch* touch, bool dropSnapped) {
    events::FinishFreeMove().send(touch, dropSnapped);
}

void commitFreeMoveUndo(CCTouch* touch) {
    events::CommitFreeMoveUndo().send(touch);
}

void handleTap(CCTouch* touch, bool tapped, bool doubleTapped, float distance) {
    events::HandleTap().send(touch, tapped, doubleTapped, distance);
}

void resetTouchState(CCTouch* touch, bool activateControls) {
    events::ResetTouchState().send(touch, activateControls);
}

bool beginEditorTouch(CCTouch* touch) {
    bool ret = false;
    events::BeginEditorTouch().send(ret, touch);
    return ret;
}

void moveEditorTouch(CCTouch* touch) {
    events::MoveEditorTouch().send(touch);
}

void endEditorTouch(CCTouch* touch) {
    events::EndEditorTouch().send(touch);
}

}