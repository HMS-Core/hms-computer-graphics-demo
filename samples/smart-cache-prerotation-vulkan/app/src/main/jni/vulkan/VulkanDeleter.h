/*
 * Copyright (C) 2019 The Huawei Source Project
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef VULKANDELETER_H
#define VULKANDELETER_H

#include <vulkan/vulkan.h>
#include <functional>

template <typename T>
class VulkanDeleter {
public:
    VulkanDeleter(const VulkanDeleter<VkInstance>& instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef) {
        this->deleter = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
    }

    VulkanDeleter(const VulkanDeleter<VkDevice>& device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef) {
        this->deleter = [&device, deletef](T obj) { deletef(device, obj, nullptr); };
    }

    VulkanDeleter() : VulkanDeleter([](T, VkAllocationCallbacks*) {}) {}

    VulkanDeleter(std::function<void(T, VkAllocationCallbacks*)> deletef) {
        this->deleter = [=](T obj) { deletef(obj, nullptr); };
    }

    ~VulkanDeleter() {
        Cleanup();
    }

    void operator=(T rhs) {
        if (rhs != object) {
            Cleanup();
            object = rhs;
        }
    }

    template<typename V>
    bool operator==(V rhs) {
        return object == T(rhs);
    }

    const T* operator &() const {
        return &object;
    }

    T* Replace() {
        Cleanup();
        return &object;
    }

    operator T() const {
        return object;
    }

    void Cleanup() {
        /*if (object != VK_NULL_HANDLE) {
            deleter(object);
        }*/
        object = VK_NULL_HANDLE;
    }

    T object{VK_NULL_HANDLE};

private:
    std::function<void(T)> deleter;

};
#endif