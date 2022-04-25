#include "VariableStore.h"

/// @brief  Gets the current instance of the variable store
/// @return The singleton instance
VariableStore& VariableStore::GetInstance()
{
    static VariableStore instance;

    return instance;
}

VariableStore::VariableStore()
{
    for (int i = 0; i < 20; i++)
    {
        variables[i] = nullptr;
    }
}
