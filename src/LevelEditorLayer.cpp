#include "LevelEditorLayer.hpp"
#include "TouchHandler.hpp"

void TouchLevelEditorLayer::forEachObject(geode::Function<void(GameObject*)> callback) {
    auto game = GJBaseGameLayer::get();

    int count = game->m_sections.empty() ? -1 : game->m_sections.size();
    for (int i = game->m_leftSectionIndex; i <= game->m_rightSectionIndex && i < count; ++i) {
        auto leftSection = game->m_sections[i];
        if (!leftSection) continue;

        auto leftSectionSize = leftSection->size();
        for (int j = game->m_bottomSectionIndex; j <= game->m_topSectionIndex && j < leftSectionSize; ++j) {
            auto section = leftSection->at(j);
            if (!section) continue;

            auto sectionSize = game->m_sectionSizes[i]->at(j);
            for (int k = 0; k < sectionSize; ++k) {
                auto obj = section->at(k);
                if (!obj) continue;

                callback(obj);
            }
        }
    }
}

CCArray* TouchLevelEditorLayer::objectsInRect(CCRect rect, bool ignoreLayerCheck) {
    auto result = CCArray::create();

    auto center = rect.origin + CCPoint(rect.size.width * 0.5f, rect.size.height * 0.5f);
    auto selectionOBB = OBB2D::create(center, rect.size.width, rect.size.height, 0.f);
    
    auto winSize = CCDirector::get()->getWinSize();

    auto centerInObjectLayer = m_objectLayer->convertToNodeSpace(winSize / 2.f);

    forEachObject([this, result, selectionOBB, centerInObjectLayer, ignoreLayerCheck] (GameObject* object) {
        if (!validGroup(object, !ignoreLayerCheck)) return;

        if (selectionOBB->overlaps(rotatedOBB2D(object, centerInObjectLayer, m_gameState.m_cameraAngle))) {
            result->addObject(object);
        }
    });

    return result;
}

OBB2D* TouchLevelEditorLayer::rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees) {
    auto obb = OBB2D::create(object->getPosition(), object->boundingBox().size.width, object->boundingBox().size.height, 0.f);

    auto center = obb->m_center;

    float width = object->boundingBox().size.width;
    float height = object->boundingBox().size.height;

    float radians = -CC_DEGREES_TO_RADIANS(degrees);
    float dx = center.x - pivot.x;
    float dy = center.y - pivot.y;

    auto rotatedCenter = CCPoint{
        pivot.x + dx * std::cos(radians) - dy * std::sin(radians),
        pivot.y + dx * std::sin(radians) + dy * std::cos(radians)
    };

    return OBB2D::create(rotatedCenter, width, height, radians);
}