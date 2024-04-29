#pragma once

#include "engine/renderer/gl/data_types.h"

namespace gfx::shape {

// clang-format off
static const float QuadVertices[] = {
    // positions   // texture coords
    -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f
};

// static const float CubeVertices[] = {
//         // positions          // normals           // texture coords
//         -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//          0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
//          0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
//          0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//
//         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//          0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
//          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
//          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//
//         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//         -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
//         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//         -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
//         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//
//          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//          0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
//          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
//          0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//
//         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
//          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
//          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//
//         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
//          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
//          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
//          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
//     };

// static const float CubeVertices[] = {
//
// };

constexpr const Vertex CubeVertices[] = {
    {{-0.5f, -0.5f, -0.5f,},{0.0f,  0.0f, -1.0f,  },{0.0f,  0.0f,},},  // 0
    {{0.5f, -0.5f, -0.5f, },{0.0f,  0.0f, -1.0f,  },{1.0f,  0.0f,},}, // 1
    {{0.5f,  0.5f, -0.5f, },{0.0f,  0.0f, -1.0f,  },{1.0f,  1.0f,},}, // 2
    {{0.5f,  0.5f, -0.5f, },{0.0f,  0.0f, -1.0f,  },{1.0f,  1.0f,},}, // 2
    {{-0.5f,  0.5f, -0.5f, },{0.0f,  0.0f, -1.0f,  },{0.0f,  1.0f,},}, // 3
    {{-0.5f, -0.5f, -0.5f, },{0.0f,  0.0f, -1.0f,  },{0.0f,  0.0f,},}, // 0

    {{-0.5f, -0.5f,  0.5f, },{0.0f,  0.0f,  1.0f,  },{0.0f,  0.0f,},}, // 4
    {{0.5f, -0.5f,  0.5f, },{0.0f,  0.0f,  1.0f,  },{1.0f,  0.0f,},}, // 5
    {{0.5f,  0.5f,  0.5f, },{0.0f,  0.0f,  1.0f,  },{1.0f,  1.0f,},}, // 6
    {{0.5f,  0.5f,  0.5f, },{0.0f,  0.0f,  1.0f,  },{1.0f,  1.0f,},}, // 6
    {{-0.5f,  0.5f,  0.5f, },{0.0f,  0.0f,  1.0f,  },{0.0f,  1.0f,},}, // 7
    {{-0.5f, -0.5f,  0.5f, },{0.0f,  0.0f,  1.0f,  },{0.0f,  0.0f,},}, // 5

    {{-0.5f,  0.5f,  0.5f, },{-1.0f,  0.0f,  0.0f,  },{1.0f,  0.0f,},},
    {{-0.5f,  0.5f, -0.5f, },{-1.0f,  0.0f,  0.0f,  },{1.0f,  1.0f,},},
    {{-0.5f, -0.5f, -0.5f, },{-1.0f,  0.0f,  0.0f,  },{0.0f,  1.0f,},},
    {{-0.5f, -0.5f, -0.5f, },{-1.0f,  0.0f,  0.0f,  },{0.0f,  1.0f,},},
    {{-0.5f, -0.5f,  0.5f, },{-1.0f,  0.0f,  0.0f,  },{0.0f,  0.0f,},},
    {{-0.5f,  0.5f,  0.5f, },{-1.0f,  0.0f,  0.0f,  },{1.0f,  0.0f,},},

    {{0.5f,  0.5f,  0.5f,  },{1.0f,  0.0f,  0.0f,  },{1.0f,  0.0f,},},
    {{0.5f,  0.5f, -0.5f,  },{1.0f,  0.0f,  0.0f,  },{1.0f,  1.0f,},},
    {{0.5f, -0.5f, -0.5f,  },{1.0f,  0.0f,  0.0f,  },{0.0f,  1.0f,},},
    {{0.5f, -0.5f, -0.5f,  },{1.0f,  0.0f,  0.0f,  },{0.0f,  1.0f,},},
    {{0.5f, -0.5f,  0.5f,  },{1.0f,  0.0f,  0.0f,  },{0.0f,  0.0f,},},
    {{0.5f,  0.5f,  0.5f,  },{1.0f,  0.0f,  0.0f,  },{1.0f,  0.0f,},},

    {{-0.5f, -0.5f, -0.5f,  },{0.0f, -1.0f,  0.0f,  },{0.0f,  1.0f,},},
    {{0.5f, -0.5f, -0.5f,  },{0.0f, -1.0f,  0.0f,  },{1.0f,  1.0f,},},
    {{0.5f, -0.5f,  0.5f,  },{0.0f, -1.0f,  0.0f,  },{1.0f,  0.0f,},},
    {{0.5f, -0.5f,  0.5f,  },{0.0f, -1.0f,  0.0f,  },{1.0f,  0.0f,},},
    {{-0.5f, -0.5f,  0.5f,  },{0.0f, -1.0f,  0.0f,  },{0.0f,  0.0f,},},
    {{-0.5f, -0.5f, -0.5f,  },{0.0f, -1.0f,  0.0f,  },{0.0f,  1.0f,},},

    {{-0.5f,  0.5f, -0.5f,  },{0.0f,  1.0f,  0.0f,  },{0.0f,  1.0f,},},
    {{0.5f,  0.5f, -0.5f,  },{0.0f,  1.0f,  0.0f,  },{1.0f,  1.0f,},},
    {{0.5f,  0.5f,  0.5f,  },{0.0f,  1.0f,  0.0f,  },{1.0f,  0.0f,},},
    {{0.5f,  0.5f,  0.5f,  },{0.0f,  1.0f,  0.0f,  },{1.0f,  0.0f,},},
    {{-0.5f,  0.5f,  0.5f,  },{0.0f,  1.0f,  0.0f,  },{0.0f,  0.0f,},},
    {{-0.5f,  0.5f, -0.5f,  },{0.0f,  1.0f,  0.0f,  },{0.0f,  1.0f},},

};

constexpr const Index CubeIndices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 5,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
};

// clang-format on

}  // namespace gfx::shape
