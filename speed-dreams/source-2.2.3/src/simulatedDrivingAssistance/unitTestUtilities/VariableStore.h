/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once

#define VARIABLE_STORE_LENGTH 20
/// @brief Stores variables that might be out of reach for tests
class VariableStore
{
public:
    static VariableStore& GetInstance();

    void* Variables[VARIABLE_STORE_LENGTH] = {};

private:
    VariableStore();

public:
    VariableStore(VariableStore const&) = delete;
    void operator=(VariableStore const&) = delete;
};