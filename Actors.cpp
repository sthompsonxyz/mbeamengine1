#pragma once

#include "Actors.h"

//#include <vulkan/vulkan.h>
//#include <vector>
//#include "vulkantypes.h"
#include "vulkanresourcehelpers.h"

//#include "kbctrler.h"
//#include "controller.h"




namespace Actors {

    BatPlayerActor2::BatPlayerActor2(GameRect gamerect_, glm::vec2 vel, glm::vec3 ids) :
        Actor3::Actor3() {}

    BatPlayerActor2::BatPlayerActor2(std::vector<Vertex>& inrenderverts, std::vector<uint16_t>& indices_, glm::vec2 vel, glm::vec3 ids) :
        Actor3::Actor3() {}

    RBatPlayerActor2::RBatPlayerActor2(GameRect gamerect_, glm::vec2 vel, glm::vec3 ids) :
        Actor3::Actor3() {}

    RBatPlayerActor2::RBatPlayerActor2(std::vector<Vertex>& inrenderverts, std::vector<uint16_t>& indices_):
        Actor3::Actor3() {}

}