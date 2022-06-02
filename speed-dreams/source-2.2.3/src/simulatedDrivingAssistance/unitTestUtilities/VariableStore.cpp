#include "VariableStore.h"

/// @brief  Gets the current instance of the variable store
/// @return The singleton instance
VariableStore& VariableStore::GetInstance()
{
    static VariableStore Instance = {};

    return Instance;
}

VariableStore::VariableStore()
{
    for (int i = 0; i < VARIABLE_STORE_LENGTH; i++)
    {
        Variables[i] = nullptr;
    }
}
