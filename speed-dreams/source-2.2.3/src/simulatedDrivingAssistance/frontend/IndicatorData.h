/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once

#include "ConfigEnums.h"
#include "guifont.h"

/// @brief Represents a position on screen as percentages of the full screen.
typedef struct ScreenPosition
{
    float X, Y;
} tScreenPosition;

/// @brief The dimensions of a texture in pixels.
typedef struct tTextureDimensions
{
    float Width, Height;
} tTextureDimensions;

/// @brief Stores data related to playing a sound in-game.
///        The actual sounds are loaded from their path in the respective module.
typedef struct SoundData
{
    const char* Path;
    bool Looping;
    /// @brief How often it should loop, in seconds.
    float LoopInterval;
    /// @brief True if last frame the sound was active
    bool ActiveLastFrame;
} tSoundData;

/// @brief Stores data related to drawing a texture on the Hud.
///        The actual textures are loaded from their path in the respective module.
typedef struct TextureData
{
    const char* Path;
    tScreenPosition ScrPos;
    tTextureDimensions Dimensions;
} tTextureData;

/// @brief Stores data related to drawing text on the Hud.
typedef struct TextData
{
    const char* Text;
    GfuiFontClass* Font;
    tScreenPosition ScrPos;
} tTextData;

/// @brief  Stores data belonging to each type of indicator. This is stored with
///         pointers to the data structs to deal with the fact that it is not required
///         that all data is defined.
typedef struct IndicatorData
{
    InterventionAction Action;
    tSoundData* Sound;
    tTextureData* Texture;
    tTextData* Text;
} tIndicatorData;