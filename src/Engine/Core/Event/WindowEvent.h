#pragma once

#include "Event.h"

class DBAPI WResizeEvent : public Event
{
public:
    WResizeEvent(uint32_t width, uint32_t height) : width(width), height(height){};
    ~WResizeEvent(){};
    static EventKind sType() { return W_ResizeEvent; };
    using Event::type;
    virtual EventKind type() { return sType(); }

    uint32_t width, height;

private:
    EventKind kind = W_ResizeEvent;
};

class DBAPI WMoveEvent : public Event
{
public:
    WMoveEvent(uint32_t x, uint32_t y) : x(x), y(y){};
    ~WMoveEvent(){};
    static EventKind sType() { return W_MoveEvent; };
    using Event::type;
    virtual EventKind type() { return sType(); }

    uint32_t x, y;

private:
    EventKind kind = W_MoveEvent;
};

class DBAPI WMinimizeEvent : public Event
{
public:
    WMinimizeEvent(){};
    ~WMinimizeEvent(){};
    static EventKind sType() { return W_MinimizeEvent; };
    using Event::type;
    virtual EventKind type() { return sType(); }

private:
    EventKind kind = W_MinimizeEvent;
};

class DBAPI WCloseEvent : public Event
{
public:
    WCloseEvent(){};
    ~WCloseEvent(){};
    static EventKind sType() { return W_CloseEvent; };
    using Event::type;
    virtual EventKind type() { return sType(); }

private:
    EventKind kind = W_CloseEvent;
};