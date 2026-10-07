#pragma once
// Observer.h - Observer pattern: the Room (subject) tells listeners when something happens.
#include <string>

#include "Item.h"

class RoomObserver {
public:
    virtual ~RoomObserver() = default;
    virtual void onObjectUnlocked(const std::string& objectId, const std::string& objectName) = 0;
    virtual void onItemFound(const Item& item) = 0;
    virtual void onRoomEscaped() = 0;
};
