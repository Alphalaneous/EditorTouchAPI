#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <asp/time/SystemTime.hpp>

using namespace geode::prelude;

class $modify(TouchLevelEditorLayer, LevelEditorLayer) {

    CCArray* objectsInRect(CCRect rect, bool ignoreLayerCheck);
    OBB2D* rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees);
    static void forEachObject(geode::Function<void(GameObject*)> callback);
};