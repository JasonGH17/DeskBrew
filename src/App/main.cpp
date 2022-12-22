#include <iostream>
#include <Engine/Core/Event/EventController.h>
#include <Engine/Graphics/Vk.h>

int main()
{
    EventController events;
    VkWindow window = VkWindow(&events);

    return 0;
}