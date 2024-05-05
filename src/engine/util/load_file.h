#pragma once

#include <optional>
#include <string>

namespace engine::util {
extern std::optional<std::string> LoadFromFile(const std::string& path);
}
