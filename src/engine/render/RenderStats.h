#pragma once

#include "impl/Export.hpp"

namespace Renderer {
    // Render stats maintained by SimpleRenderEngine
    struct DllExport RenderStats {
        int frame;
        int meshCount;
        int meshBytes;
        int textureCount;
        int textureBytes;
        int shaderCount;
        int drawCalls;
    };
}
