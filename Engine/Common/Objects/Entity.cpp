//
// Created by Remus on 22/12/2023.
//

#include "Entity.h"
#include <vector>
#include "Components/Component.h"


void Entity::Construct() {
    for (const auto &component: mComponentMap) {
        component.second->Initialize();
    }
}

void Entity::Tick(const float aDeltaTime) {
    for (const auto &component: mComponentMap) {
        component.second->Tick(aDeltaTime);
    }
}

void Entity::Cleanup() {

    const std::vector<std::pair<std::string, Component*>> copy(mComponentMap.begin(), mComponentMap.end());
    for (const auto &component: copy) {
        component.second->Destroy();
        delete component.second;
    }
}

void Entity::AddComponent(Component *aComponent) {
    aComponent->SetEntity(this);
    mComponentMap[aComponent->GetName()] = aComponent;
}

void Entity::RemoveComponent(Component *aComponent) {
    if (const auto it = mComponentMap.find(aComponent->GetName()); it != mComponentMap.end()) {
        mComponentMap.erase(it);
    }
}

void Entity::OnImGuiRender() {
    transform.OnImGuiRender();
}
