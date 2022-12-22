#pragma once

#include <functional>

#include "Event.h"
#include "Core/Logger/Logger.h"

#include "api.h"

class DBAPI EventController
{
public:
    EventController()
    {
        for (size_t i = 0; i < EVENT_KIND_MAX; i++)
        {
            nCallbacks[i] = 0;
            for (size_t j = 0; j < 64; j++)
            {
                callbacks[i][j] = nullptr;
            }
        };
    };
    ~EventController(){};

    template <class T>
    uint32_t addEventListener(std::function<void(T *)> callback)
    {
        static_assert(std::is_base_of<Event, T>::value, "Invalid event (Must derive Event base class)");
        // Replace with faster search algorithm if needed later on
        for (size_t i = 0; i < 64; i++)
        {
            if (callbacks[T::sType()][i] == nullptr)
            {
                callbacks[T::sType()][i] = [callback](Event *e)
                { callback((T*)e); };
                nCallbacks[T::sType()]++;
                return i;
            }
        }

        DBFatal(DBEvent, "Reached max event listeners for event: %d", T::sType());
        return 0;
    }

    template <typename T>
    void removeEventListener(uint32_t id)
    {
        static_assert(std::is_base_of<Event, T>::value, "Invalid event (Must derive Event base class)");
        callbacks[T::sType()][id] = nullptr;
        nCallbacks[T::sType()]--;
    }

    template <typename T>
    void dispatchEvent(T *e)
    {
        uint32_t calls = 0;
        for (size_t i = 0; i < 64; i++)
        {
            if (callbacks[e->type()][i] != nullptr)
            {
                callbacks[e->type()][i](e);
                calls++;
                if (calls >= nCallbacks[e->type()])
                    break;
            }
        }
    }

private:
    std::function<void(Event *)> callbacks[EVENT_KIND_MAX][64];
    uint32_t nCallbacks[EVENT_KIND_MAX];
};