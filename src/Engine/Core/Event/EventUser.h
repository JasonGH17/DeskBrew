#pragma once

#include "EventController.h"
#include "api.h"

class DBAPI EventUser
{
public:
    void bindES(EventController *events) { e = events; };
    EventController *events()
    {
        if (e != nullptr)
            return e;

        DBFatal(DBEvent, "No event system was bound to the object...");
        return nullptr;
    };

protected:
    EventUser(EventController *events){
        bindES(events);
    };
    ~EventUser(){};

private:
    EventController *e;
};
