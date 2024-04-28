#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "core/e_assert.h"
#include "util/hashed_string.h"
