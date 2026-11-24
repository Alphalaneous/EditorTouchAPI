#include "EditorUI.hpp"
#include "TouchHandler.hpp"
#include "../include/Events.hpp"
#include "../include/API.hpp"

using namespace alpha::editor_touch;

void TouchEditorUI::setupEvents() {
    addEventListener(events::SnapToGrid(), [this] (CCPoint& ret, cocos2d::CCTouch*& touch) {
        auto point = m_editorLayer->m_objectLayer->convertToNodeSpace(touch->getLocation());
        
        float grid = m_editorLayer->m_drawGridLayer->m_gridSize;
        float rowOffset = std::fmod(90.f, grid);

        float cellY = std::floor((point.y - rowOffset) / grid);
        float cellX = std::floor(point.x / grid);

        ret = getLimitedPosition({
            (cellX + 0.5f) * grid,
            (cellY + 0.5f) * grid + rowOffset
        });
    });

    addEventListener(events::StopSwipeTrigger(), [this] (cocos2d::CCTouch*& touch) {
        stopActionByTag(123);
    });

    addEventListener(events::CanFreeMove(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        if (m_spaceSwiping) {
            ret = false;
            return;
        }

        auto keyboard = CCKeyboardDispatcher::get();

        if (!keyboard->getShiftKeyPressed() && !m_freeMoveEnabled && !keyboard->getControlKeyPressed() && !keyboard->getCommandKeyPressed()) {
            ret = false;
            return;
        }

        ret = (m_selectedMode == 3);
    });

    addEventListener(events::RefreshSelectionUI(), [this] (cocos2d::CCTouch*& touch) {
        if (!m_selectedObject && m_selectedObjects->count() == 0) {
            deactivateRotationControl();
            deactivateScaleControl();
            deactivateTransformControl();
        }

        updateObjectInfoLabel();
        updateScaleControl();
    });

    addEventListener(events::GetSelectableObject(), [this] (GameObject*& ret, cocos2d::CCTouch*& touch, cocos2d::CCArray*& objects) {
        GameObject* picked = nullptr;

        for (auto object : CCArrayExt<GameObject, false>(objects)) {
            if (object->m_isSelected || (picked && object->m_cycleIndex >= picked->m_cycleIndex)) {
                continue;
            }

            if (canSelectObject(object)) {
                picked = object;
            }
        }

        ret = picked;
    });

    addEventListener(events::SelectPickedObject(), [this] (cocos2d::CCTouch*& touch, GameObject*& object) {
        if (!object) return;

        object->m_cycleIndex = m_cycledObjectIndex;
        createUndoSelectObject(false);

        if (!m_swipeActive) {
            selectObject(object, false);
        }
        else {
            selectObjects(CCArray::createWithObject(object), false);
        }

        EditorSFXManager::sharedState()->playAction(121);
    });
    
    addEventListener(events::DeselectLastObject(), [this] (cocos2d::CCTouch*& touch) {
        if (m_selectedObject) {
            stopActionByTag(124);

            m_selectedObject->m_isSelected = false;
            m_selectedObject->updateObjectEditorColor();

            m_selectedObject = nullptr;
            toggleEditObjectButton();
            m_canActivateControls = true;
            m_lastTouchPoint = CCPoint{};
        }
        else if (auto last = static_cast<GameObject*>(m_selectedObjects->lastObject())) {
            last->m_isSelected = false;
            last->updateObjectEditorColor();
            m_selectedObjects->removeLastObject(true);
        }
    });

    addEventListener(events::FindSnapObjectAtPos(), [this] (GameObject*& ret, cocos2d::CCTouch*& touch) {
        auto objects = m_editorLayer->objectsAtPosition(m_editorLayer->m_objectLayer->convertToNodeSpace(touch->getLocation()));

        for (auto object : CCArrayExt<GameObject, false>(objects)) {
            if (object->m_uniqueID == m_selectedObject->m_uniqueID) {
                ret = object;
                return;
            }
        }
        ret = nullptr;
    });

    addEventListener(events::InitializeSnapObject(), [this] (cocos2d::CCTouch*& touch) {
        auto point = touch->getLocation();
        
        m_snapSelected = false;
        m_snapObject = nullptr;

        if (CCKeyboardDispatcher::get()->getAltKeyPressed() && (m_selectedObject || m_selectedObjects->count() != 0)) {
            onDuplicate(nullptr);
            m_noSnapUndo = true;
        }

        m_findSnap = false;

        if (m_selectedObjects->count() != 0) {
            auto nodeSpace = m_editorLayer->m_objectLayer->convertToNodeSpace(touch->getLocation());
            findSnapObject(nodeSpace, 0.f);
        }
        else if (!m_selectedObject) {
            m_editingObject = true;
            clickOnPosition(m_swipeStart);

            if (m_snapSelected) {
                m_snapObject = m_selectedObject;
            }
        }
        else {
            m_snapObject = findSnapObjectAtPos(touch);

            if (!m_snapObject && !m_swipeEnabled) {
                m_editingObject = true;
                clickOnPosition(m_swipeStart);

                if (m_snapSelected) {
                    m_snapObject = m_selectedObject;
                }
            }
        }

        if (m_findSnap && !m_snapObject && m_selectedObjects->count() != 0) {
            findSnapObject(point, 0.f);
        }

        if (m_snapObject) {
            m_snapObjectExists = true;
            m_snapPosition = m_snapObject->getPosition();
        }
    });

    addEventListener(events::CreateFreeMoveUndo(), [this] (cocos2d::CCTouch*& touch) {
        if (m_noSnapUndo) return;

        auto undo = createUndoObject(UndoCommand::Transform, true);

        if (m_undoObject != undo) {
            undo->retain();
            if (m_undoObject) {
                m_undoObject->release();
            }
            m_undoObject = undo;
        }
    });

    addEventListener(events::BeginFreeMove(), [this] (cocos2d::CCTouch*& touch) {
        initializeSnapObject(touch);

        m_cameraTest = m_editorLayer->m_objectLayer->convertToNodeSpace(m_swipeStart);
        createFreeMoveUndo(touch);
    });

    addEventListener(events::HandleTouchControlsBegin(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        if (m_rotationTouchID == -1 && m_rotationControl->isVisible() && m_rotationControl->ccTouchBegan(touch, nullptr)) {
            m_rotationTouchID = touch->getID();
            ret = true;
            return;
        }

        if (m_scaleTouchID == -1 && m_scaleControl->isVisible() && m_scaleControl->ccTouchBegan(touch, nullptr)) {
            m_scaleTouchID = touch->getID();
            ret = true;
            return;
        }

        if (m_transformTouchID == -1 && m_transformControl->isVisible() && m_transformControl->ccTouchBegan(touch, nullptr)) {
            m_transformTouchID = touch->getID();
            ret = true;
            return;
        }

        ret = false;
    });

    addEventListener(events::HandleTouchControlsMove(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        int id = touch->getID();

        if (m_rotationTouchID == id) {
            stopSwipeTrigger(touch);
            m_rotationControl->ccTouchMoved(touch, nullptr);
            ret = true;
            return;
        }

        if (m_scaleTouchID == id) {
            stopSwipeTrigger(touch);
            m_scaleControl->ccTouchMoved(touch, nullptr);
            ret = true;
            return;
        }

        if (m_transformTouchID == id) {
            stopSwipeTrigger(touch);
            m_transformControl->ccTouchMoved(touch, nullptr);
            ret = true;
            return;
        }

        ret = false;
    });

    addEventListener(events::HandleTouchControlsEnd(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        int id = touch->getID();

        if (m_rotationTouchID == id) {
            m_rotationTouchID = -1;
            m_rotationControl->ccTouchEnded(touch, nullptr);
            ret = true;
            return;
        }

        if (m_scaleTouchID == id) {
            m_scaleTouchID = -1;
            m_scaleControl->ccTouchEnded(touch, nullptr);
            ret = true;
            return;
        }

        if (m_transformTouchID == id) {
            m_transformTouchID = -1;
            m_transformControl->ccTouchEnded(touch, nullptr);
            ret = true;
            return;
        }

        ret = false;
    });

    addEventListener(events::BeginPinchGesture(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        if (m_touchID == -1 || m_unk2fc != -1 || m_unk31c) {
            ret = false;
            return;
        }

        if (GameManager::get()->getGameVariable(GameVar::DisablePinch) || m_snapObjectExists || m_swipeActive || m_swipeSelected) {
            ret = false;
            return;
        }

        m_unk2fc = touch->getID();
        m_unkPoint308 = touch->getLocation();
        m_unk310 = m_unkPoint300.getDistance(m_unkPoint308);
        m_unk314 = 24.5f;
        m_unk318 = m_editorLayer->m_objectLayer->getScale();
        m_unk31c = true;
        m_unk31d = false;

        stopSwipeTrigger(touch);

        m_swipeActive = false;
        m_swipeSelected = false;

        ret = true;
    });

    addEventListener(events::MovePinchGesture(), [this] (cocos2d::CCTouch*& touch) {
        auto previousMid = (m_unkPoint300 + m_unkPoint308) / 2.f;
        auto point = touch->getLocation();

        if (touch->getID() == m_touchID) {
            m_unkPoint300 = point;
        }
        else if (touch->getID() == m_unk2fc) {
            m_unkPoint308 = point;
        }

        auto mid = (m_unkPoint300 + m_unkPoint308) / 2.f;

        auto objectLayer = m_editorLayer->m_objectLayer;
        objectLayer->setPosition(objectLayer->getPosition() + (mid - previousMid));

        float distance = m_unkPoint300.getDistance(m_unkPoint308);

        if (!m_unk31d) {
            if (distance <= 1.f || std::abs(distance - m_unk310) < m_unk314) {
                constrainGameLayerPosition(-100.f, -100.f);
                updateSlider();
                return;
            }

            m_unk31d = true;
            m_unk310 = distance;
        }

        float zoom = distance / m_unk310 * m_unk318;
        zoom = std::min(zoom, 10.f);

        if (zoom <= 0.1f) zoom = 0.1f;
        if (std::abs(zoom - 1.f) <= 0.02f) zoom = 1.f;

        updateZoom(zoom, mid);
    });

    addEventListener(events::EndPinchGesture(), [this] (cocos2d::CCTouch*& touch) {
        m_unk2fc = -1;
        m_touchID = -1;
        m_unk31c = false;
        m_isDraggingCamera = false;
    });

    addEventListener(events::SwipeTouchMoved(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        if (m_spaceSwiping) {
            ret = false;
            return;
        }

        auto point = touch->getLocation();
        auto keyboard = CCKeyboardDispatcher::get();

        if (!m_swipeEnabled && !keyboard->getShiftKeyPressed() && !m_swipeModeTriggered) {
            ret = false;
            return;
        }

        if (m_selectedMode == 3) {
            m_swipeEnd = point;
        }
        else {
            m_createPosition = snapToGrid(touch);
            clickOnPosition(point);
        }

        stopSwipeTrigger(touch);
        ret = true;
    });

    addEventListener(events::SelectSwipeRectangle(), [this] (cocos2d::CCTouch*& touch) {
        auto objectLayer = m_editorLayer->m_objectLayer;

        auto start = objectLayer->convertToNodeSpace(alpha::editor_touch::TouchHandler::rotatePoint(m_swipeStart, true));
        auto end = objectLayer->convertToNodeSpace(alpha::editor_touch::TouchHandler::rotatePoint(m_swipeEnd, true));

        auto rect = CCRect{
            std::min(start.x, end.x),
            std::min(start.y, end.y),
            std::abs(start.x - end.x),
            std::abs(start.y - end.y)
        };

        auto objects = m_editorLayer->objectsInRect(rect, false);

        if (objects->count() == 0) return;

        EditorSFXManager::sharedState()->playAction(122);

        for (auto object : CCArrayExt<GameObject, false>(objects)) {
            if ((m_selectedObject && m_selectedObject->m_uniqueID != object->m_uniqueID) || !m_selectedObjects->containsObject(object)) {
                createUndoSelectObject(false);
                break;
            }
        }

        selectObjects(objects, false);
        updateObjectInfoLabel();
        updateScaleControl();
    });

    addEventListener(events::SelectSwipeTap(), [this] (cocos2d::CCTouch*& touch) {
        auto objectLayer = m_editorLayer->m_objectLayer;
        auto end = objectLayer->convertToNodeSpace(m_swipeEnd);

        auto objects = m_editorLayer->objectsAtPosition(end);
        selectPickedObject(touch, getSelectableObject(touch, objects));
    });

    addEventListener(events::HandleSwipeSelection(), [this] (cocos2d::CCTouch*& touch, bool& tapped) {
        if (!m_swipeActive || tapped) return;

        if (m_swipeStart.getDistance(touch->getLocation()) < 2.f) {
            selectSwipeTap(touch);
        }
        else {
            selectSwipeRectangle(touch);
        }
    });

    addEventListener(events::MoveSelectionBy(), [this] (cocos2d::CCTouch*& touch, cocos2d::CCPoint& offset) {
        if (m_selectedObjects->count() == 0) {
            moveObject(m_selectedObject, offset);
            return;
        }
        for (auto object : CCArrayExt<GameObject, false>(m_selectedObjects)) {
            moveObject(object, offset);
        }
    });

    addEventListener(events::MoveTransformControls(), [this] (cocos2d::CCTouch*& touch, cocos2d::CCPoint& offset) {
        m_pivotPoint = m_pivotPoint + offset;

        if (m_rotationControl->isVisible()) {
            m_rotationControl->setPosition(m_rotationControl->getPosition() + offset);
        }
        else if (m_scaleControl->isVisible()) {
            m_scaleControl->setPosition(m_scaleControl->getPosition() + offset);
        }
        else if (m_transformControl->isVisible()) {
            m_transformControl->setPosition(m_transformControl->getPosition() + offset);
            m_transformControl->refreshControl();
        }
    });

    addEventListener(events::ShouldContinueSwipe(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        if (m_continueSwipe) {
            ret = true;
            return;
        }

        if (touch->getLocation().getDistance(m_swipeStart) >= 4.f) {
            m_continueSwipe = true;
        }

        ret = m_continueSwipe;
    });

    addEventListener(events::MoveFreeMoveObject(), [this] (cocos2d::CCTouch*& touch) {
        if (!shouldContinueSwipe(touch)) {
            stopSwipeTrigger(touch);
            return;
        }

        auto point = m_editorLayer->m_objectLayer->convertToNodeSpace(touch->getLocation());

        if (point.equals(m_cameraTest)) {
            stopSwipeTrigger(touch);
            return;
        }

        if (!m_continuousSnap) {
            EditorSFXManager::sharedState()->playAction(104);
        }

        if (!m_continuousSnap && m_snapObject && GameManager::get()->getGameVariable(GameVar::EnableSnap) && isRightAngle(touch, m_snapObject->getRotation())) {
            m_snapObject->setObjectColor({255, 155, 50});
        }

        m_continuousSnap = true;

        auto offset = point - m_cameraTest;

        moveSelectionBy(touch, offset);
        m_cameraTest = point;
        moveTransformControls(touch, offset);

        stopSwipeTrigger(touch);
    });

    addEventListener(events::MoveCamera(), [this] (cocos2d::CCTouch*& touch) {
        auto point = touch->getLocation();

        if (!m_isDraggingCamera) {
            if (point.getDistance(m_swipeStart) < 20.f) return;

            m_cameraTest = point;
            m_isDraggingCamera = true;
            stopSwipeTrigger(touch);
            return;
        }

        auto offset = point - m_cameraTest;

        auto objectLayer = m_editorLayer->m_objectLayer;
        objectLayer->setPosition(objectLayer->getPosition() + offset);

        constrainGameLayerPosition(-100.f, -100.f);

        m_cameraTest = point;
        updateSlider();
    });

    addEventListener(events::SnapFreeMoveObject(), [this] (bool& ret, cocos2d::CCTouch*& touch, cocos2d::CCPoint& offset) {
        if (!m_snapObjectExists || !m_continuousSnap || !m_snapObject) {
            ret = false;
            return;
        }

        auto point = touch->getLocation();

        auto object = m_snapObject;

        if (!GameManager::get()->getGameVariable(GameVar::EnableSnap) || !isRightAngle(touch, object->getRotation()) || point.equals(m_swipeStart)) {
            ret = false;
            return;
        }

        auto snapped = snapToGrid(touch);
        auto relative = GameToolbox::getRelativeOffset(object, offsetForKey(object->m_objectID));

        snapped = snapped + applySpecialOffset(relative, object, snapped);

        auto previous = object->getPosition();
        object->setPosition(snapped);

        offset = snapped - previous;

        if (m_selectedObjects->count() != 0) {
            for (auto selected : CCArrayExt<GameObject, false>(m_selectedObjects)) {
                if (selected->m_uniqueID != object->m_uniqueID) {
                    moveObject(selected, offset);
                }
            }
        }

        moveTransformControls(touch, offset);
        ret = true;
    });

    addEventListener(events::FinishFreeMove(), [this] (cocos2d::CCTouch*& touch, bool& dropSnapped) {
        if (!m_snapObject || !m_snapObjectExists || !m_continuousSnap) return;

        EditorSFXManager::sharedState()->playAction(dropSnapped ? 106 : 105);

        if (m_snapObject->m_isSelected) {
            m_snapObject->selectObject({0, 255, 0});
        }
        else {
            m_snapObject->m_isSelected = false;
            m_snapObject->updateObjectEditorColor();
        }
    });

    addEventListener(events::CommitFreeMoveUndo(), [this] (cocos2d::CCTouch*& touch) {
        if (!m_snapObject || !m_snapObjectExists || !m_continuousSnap) return;

        if (m_snapObject->getPosition().equals(m_snapPosition)) {
            m_noSnapUndo = true;
        }

        if (!m_noSnapUndo && m_undoObject) {
            m_editorLayer->addToUndoList(m_undoObject, false);

            m_undoObject->release();
            m_undoObject = nullptr;
        }
    });

    addEventListener(events::BeginEditorTouch(), [this] (bool& ret, cocos2d::CCTouch*& touch) {
        m_swipeStart = touch->getLocation();

        if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
            m_editorLayer->m_uiLayer->ccTouchBegan(touch, nullptr);
            ret = true;
            return;
        }

        if (m_touchID != -1) {
            ret = false;
            return;
        }

        if (m_undoObject) {
            m_undoObject->release();
            m_undoObject = nullptr;
        }

        auto keyboard = CCKeyboardDispatcher::get();
        bool swipeModifier = keyboard->getShiftKeyPressed();

        if (!m_spaceSwiping && (m_swipeEnabled || swipeModifier) && m_selectedMode == 3) {
            m_swipeActive = true;
            m_swipeEnd = m_swipeStart;
        }
        else if (canFreeMove(touch)) {
            beginFreeMove(touch);
        }

        m_swipeSelected = m_swipeEnabled || swipeModifier;
        m_continuousSnap = false;
        m_touchID = touch->getID();
        m_unkPoint300 = m_swipeStart;

        stopSwipeTrigger(touch);

        if (!m_swipeSelected && GameManager::get()->getGameVariable(GameVar::HoldToSwipe)) {
            auto trigger = CCCallFunc::create(this, callfunc_selector(EditorUI::triggerSwipeMode));
            auto action = CCSequence::create(CCDelayTime::create(0.3f), trigger, nullptr);

            action->setTag(123);
            runAction(action);
        }

        m_keyTime = asp::time::SystemTime::now().timeSinceEpoch().seconds<double>();
        m_swipeModeTriggered = false;

        ret = true;
    });

    addEventListener(events::MoveEditorTouch(), [this] (cocos2d::CCTouch*& touch) {
        if (m_unk31c) {
            movePinchGesture(touch);
            return;
        }

        if (m_touchID != touch->getID()) return;

        m_unkPoint300 = touch->getLocation();

        if (m_snapObjectExists) {
            moveFreeMoveObject(touch);
            return;
        }

        if (swipeTouchMoved(touch)) return;

        moveCamera(touch);
    });

    addEventListener(events::EndEditorTouch(), [this] (cocos2d::CCTouch*& touch) {
        bool swipeCycle = GameManager::get()->getGameVariable(GameVar::SwipeCycleMode);

        auto point = touch->getLocation();

        bool tapped = swipeCycle && m_swipeStart.getDistance(point) < 2.f;
        bool doubleTapped = swipeCycle && m_lastTouchPoint.getDistance(point) < 2.f;

        m_touchID = -1;

        bool dropSnapped = false;
        auto offset = CCPoint{};

        if (m_snapObjectExists && m_continuousSnap && m_snapObject) {
            dropSnapped = snapDraggedObject(touch, offset);
            commitFreeMoveUndo(touch);
        }

        handleTap(touch, tapped, doubleTapped, m_swipeStart.getDistance(point));

        finishFreeMove(touch, dropSnapped);

        m_snapObject = nullptr;

        bool activateControls = m_canActivateControls;
        resetTouchState(touch, activateControls);
    });

    addEventListener(events::HandleTap(), [this] (cocos2d::CCTouch*& touch, bool& tapped, bool& doubleTapped, float& distance) {
        if (m_swipeActive && !tapped) {
            handleSwipeSelection(touch, tapped);
            return;
        }

        if (m_isDraggingCamera) {
            m_isDraggingCamera = false;
            return;
        }

        if (m_swipeSelected && distance >= 20.f) return;

        if (m_selectedMode != 3) {
            if (!m_swipeModeTriggered) {
                clickOnPosition(m_swipeStart);
            }
            return;
        }

        if (distance >= 20.f || (m_snapObject && m_editingObject)) return;

        auto nodePoint = m_editorLayer->m_objectLayer->convertToNodeSpace(m_swipeStart);
        auto objects = m_editorLayer->objectsAtPosition(nodePoint);
        auto picked = getSelectableObject(touch, objects);

        if (!picked) return;

        picked->m_cycleIndex = m_cycledObjectIndex;
        createUndoSelectObject(false);

        if (!m_swipeActive) {
            selectObject(picked, false);
            EditorSFXManager::sharedState()->playAction(121);
            return;
        }

        if (doubleTapped) {
            deselectLastObject(touch);
        }

        selectObjects(CCArray::createWithObject(picked), false);
    });

    addEventListener(events::ResetTouchState(), [this] (cocos2d::CCTouch*& touch, bool& activateControls) {
        m_swipeActive = false;
        m_swipeSelected = false;
        m_snapObjectExists = false;
        m_noSnapUndo = false;
        m_snapPosition = CCPoint{};
        m_createPosition = CCPoint{};
        m_continuousSnap = false;
        m_editingObject = false;
        m_swipePosition = m_swipeStart;
        m_continueSwipe = false;

        updateButtons();
        m_cycledObjectIndex++;
        m_snapPositions->removeAllObjects();

        if (activateControls) {
            refreshSelectionUI(touch);
            updateTransformControl();
            m_canActivateControls = false;
        }

        m_lastTouchPoint = touch->getLocation();
    });

    addEventListener(events::IsRightAngle(), [this] (bool& ret, cocos2d::CCTouch*& touch, float& rotation) {
        ret = std::fmod(rotation, 90.f) == 0.f;
    });
}