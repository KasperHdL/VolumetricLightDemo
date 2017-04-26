#pragma once


#include "impl/Export.hpp"

/**
 * Defines the types of Light. Note ambient light is simply represented using a vec3 in the SimpleRenderEngine class.
 */
enum class LightType {
    Point,
    Directional,
    Unused
};
