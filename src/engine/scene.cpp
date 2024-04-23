#include "scene.h"

#include <utility>

Scene::Scene(std::string name) : name_(std::move(name)) {}
