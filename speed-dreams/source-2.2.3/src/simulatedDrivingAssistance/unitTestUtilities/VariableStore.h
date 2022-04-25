#pragma once

/// @brief Stores variables that might be out of reach for tests
class VariableStore
{
public:
    static VariableStore& GetInstance();

    void* variables[20] = {};

private:
    VariableStore();

public:
    VariableStore(VariableStore const&) = delete;
    void operator=(VariableStore const&) = delete;
};