#pragma once

#include <Geode/loader/EventExport.hpp>
#include "EditorTouch.hpp"
#include "Events.hpp"

#define MY_MOD_ID "alphalaneous.editortab_api"

namespace alpha::editor_touch {

GEODE_EVENT_EXPORT(bool, isRightAngle, (cocos2d::CCTouch* touch, float rotation), (touch, rotation));
GEODE_EVENT_EXPORT(cocos2d::CCPoint, snapToGrid, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, stopSwipeTrigger, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, canFreeMove, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, refreshSelectionUI, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(GameObject*, getSelectableObject, (cocos2d::CCTouch* touch, cocos2d::CCArray* objects), (touch, objects));
GEODE_EVENT_EXPORT(void, selectPickedObject, (cocos2d::CCTouch* touch, GameObject* object), (touch, object));
GEODE_EVENT_EXPORT(void, deselectLastObject, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(GameObject*, findSnapObjectAtPos, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, initializeSnapObject, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, createFreeMoveUndo, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, beginFreeMove, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, handleTouchControlsBegin, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, handleTouchControlsMove, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, handleTouchControlsEnd, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, beginPinchGesture, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, movePinchGesture, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, endPinchGesture, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, swipeTouchMoved, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, selectSwipeRectangle, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, selectSwipeTap, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, handleSwipeSelection, (cocos2d::CCTouch* touch, bool tapped), (touch, tapped));
GEODE_EVENT_EXPORT(void, moveSelectionBy, (cocos2d::CCTouch* touch, cocos2d::CCPoint& offset), (touch, offset));
GEODE_EVENT_EXPORT(void, moveTransformControls, (cocos2d::CCTouch* touch, cocos2d::CCPoint& offset), (touch, offset));
GEODE_EVENT_EXPORT(bool, shouldContinueSwipe, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, moveFreeMoveObject, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, moveCamera, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, snapDraggedObject, (cocos2d::CCTouch* touch, cocos2d::CCPoint& offset), (touch, offset));
GEODE_EVENT_EXPORT(void, finishFreeMove, (cocos2d::CCTouch* touch, bool dropSnapped), (touch, dropSnapped));
GEODE_EVENT_EXPORT(void, commitFreeMoveUndo, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(bool, beginEditorTouch, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, moveEditorTouch, (cocos2d::CCTouch* touch), (touch));
GEODE_EVENT_EXPORT(void, handleTap, (cocos2d::CCTouch* touch, bool tapped, bool doubleTapped, float distance), (touch, tapped, doubleTapped, distance));
GEODE_EVENT_EXPORT(void, resetTouchState, (cocos2d::CCTouch* touch, bool activateControls), (touch, activateControls));
GEODE_EVENT_EXPORT(void, endEditorTouch, (cocos2d::CCTouch* touch), (touch));

}