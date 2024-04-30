#include "scene.h"

#include "engine/application/engine.h"
#include "engine/timestep.h"

Scene::Scene(std::string name) : name_(std::move(name)) {}

void Scene::OnEvent(const Event& e) {}

void Scene::OnUpdate(Timestep timestep) {}

void Scene::Shutdown() { registry.clear(); }
