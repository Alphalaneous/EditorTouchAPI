#pragma once

#include <Geode/loader/Event.hpp>
#include <alphalaneous.alphas-ui-pack/touch/Touch.hpp>

namespace alpha::editor_touch {

/// Denotes whether a touch should continue to propagate or be swallowed. This is not the same as the event 
/// itself being propagated, and is instead an alias for if ccTouchBegan returns true or false.
/// Swallow == true, Propagate == false
struct TouchAction {
    static constexpr bool Swallow = true;
    static constexpr bool Propagate = false;

    constexpr TouchAction() noexcept = default;
    constexpr TouchAction(bool value) noexcept : m_value(value) {}
    constexpr TouchAction(TouchAction const&) noexcept = default;
    constexpr TouchAction(TouchAction&&) noexcept = default;
    constexpr TouchAction& operator=(TouchAction const&) noexcept = default;
    constexpr TouchAction& operator=(TouchAction&&) noexcept = default;

    constexpr operator bool() const noexcept {
        return m_value;
    }

    bool m_value = false;
};

/// Tip: `alpha::dispatcher::Touch` has a method `getButton` which lets you check which mouse button was used.
/// 
/// Event for when the editor is first touched. It is recommended not to set ret to `TouchAction::Propagate`
/// manually and only ever set to `TouchAction::Swallow` true as needed. Setting it to `TouchAction::Propagate` can 
/// cause other mods to break if you aren't careful, use with caution
class TouchBegin : public geode::Event<TouchBegin, bool(TouchAction& ret, alpha::dispatcher::Touch* touch)> {
public:
    using Event::Event;
};

/// Tip: `alpha::dispatcher::Touch` has a method `getButton` which lets you check which mouse button was used.
/// 
/// If `TouchBegin` Swallowed, this event will run when the touch moves.
class TouchMoved : public geode::Event<TouchMoved, bool(alpha::dispatcher::Touch* touch)> {
public:
    using Event::Event;
};

/// Tip: `alpha::dispatcher::Touch` has a method `getButton` which lets you check which mouse button was used.
/// 
/// If `TouchBegin` Swallowed, this event will run when the touch ends.
class TouchEnded : public geode::Event<TouchEnded, bool(alpha::dispatcher::Touch* touch)> {
public:
    using Event::Event;
};

/// Tip: `alpha::dispatcher::Touch` has a method `getButton` which lets you check which mouse button was used.
/// 
/// If `TouchBegin` Swallowed, this event will run when the touch is cancelled.
class TouchCancelled : public geode::Event<TouchCancelled, bool(alpha::dispatcher::Touch* touch)> {
public:
    using Event::Event;
};

}