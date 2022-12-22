#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdint.h>

#include "Core/Logger/Logger.h"
#include "EventController.h"

#include "api.h"

enum EventKind
{
    W_ResizeEvent,
    W_MoveEvent,
    W_MinimizeEvent,
    W_CloseEvent,
    I_KeyboardKeyPressedEvent,
    I_KeyboardKeyReleasedEvent,
    I_MouseMoveEvent,
    I_MouseButtonPressedEvent,
    I_MouseButtonReleasedEvent,
    I_MouseScrollEvent,
    EVENT_KIND_MAX
};

class DBAPI Event
{
public:
    virtual ~Event(){};
    virtual EventKind type() const { return kind; };

protected:
    EventKind kind;
};
