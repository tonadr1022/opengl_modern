#include "scene.h"

#include "engine/application/input.h"
#include "engine/timestep.h"

Scene::Scene(std::string name) : name_(std::move(name)) {}

void Scene::OnKeyEvent(const KeyEvent& e) {}

void Scene::OnUpdate(Timestep timestep) {}
