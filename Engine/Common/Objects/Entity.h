//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "ImGuiLayer.h"
#include "Math/Transform.h"
class Component;

class Entity : public ImGuiLayer {
public:
    virtual ~Entity() = default;

    virtual void Construct();

    virtual void Tick(float aDeltaTime);

    virtual void Cleanup();

    virtual void AddComponent(Component *aComponent);

    virtual void RemoveComponent(Component *aComponent);


    template<typename T>
    bool GetComponent(T &aResult) {
        for (auto &component: mComponentMap) {
            if (dynamic_cast<T *>(component.second)) {
                aResult = *static_cast<T *>(component.second);
                return true;
            }
        }
        return false;
    }
    template<typename T>
    bool GetComponents(std::vector<T> &aResult) {
        aResult = std::vector<T>();

        for (auto &component: mComponentMap) {
            if (dynamic_cast<T *>(component.second)) {
                aResult = *static_cast<T *>(component.second);
                aResult.push_back(*static_cast<T *>(component.second));
            }
        }
        return !aResult.empty();
    }
    template<class T>
    bool GetComponent(const char* name, T &aResult) {
        if (mComponentMap.contains(name)) {
            aResult = *static_cast<T*>(mComponentMap[name]);  // add static_cast here
            return true;
        }
        return false;
    }

    template<class T>
    bool GetComponent(const std::string name, T &aResult) {
        if (mComponentMap.contains(name)) {
            aResult = *static_cast<T*>(mComponentMap[name]);  // add static_cast here
            return true;
        }
        return false;
    }


    void OnImGuiRender() override;

    Transform transform;
    const char *mName{"Default"};

    std::unordered_map<std::string, Component *> mComponentMap;
};
