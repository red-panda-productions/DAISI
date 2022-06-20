/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

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
