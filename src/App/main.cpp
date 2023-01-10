#include <iostream>
#include <Engine/Core/Event/EventController.h>
#include <Engine/Graphics/Vk/Vk.h>
// #include <Engine/Graphics/OGL/OGL.h>

int main()
{
    EventController events;
    VkWindow window = VkWindow(&events);
    // OGL window = OGL(&events);

    return 0;
}