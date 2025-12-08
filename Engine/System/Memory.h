//
// Created by remus on 8/12/2025.
//
#pragma once

#include <cstddef>
#include <new>

#pragma once

class Memory
{
public:
    static void* AllocImpl(size_t size, const char* name = nullptr);
    static void FreeImpl(void* ptr);
};