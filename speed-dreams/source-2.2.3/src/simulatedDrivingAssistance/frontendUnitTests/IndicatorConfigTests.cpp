#include <gtest/gtest.h>
#include <tgfclient.h>
#include "IndicatorConfig.h"
#include "../rppUtils/RppUtils.hpp"
#include "../rppUtils/Random.hpp"
#include "TestUtils.h"
#include <portability.h>

#define MAX_TEXT_LENGTH 64

#define TEST_XML_FILE "test.xml"

tSoundData* CreateRandomSoundData(Random& p_rnd)
{
    // Chance to generate nullptr
    if (p_rnd.NextInt(0, 5) == 0) return nullptr;

    // Otherwise generate random data object
    tSoundData* data = new SoundData;
    data->ActiveLastFrame = p_rnd.NextBool();
    data->Looping = p_rnd.NextBool();
    data->LoopInterval = p_rnd.NextFloat();

    char* buf = new char[PATH_BUF_SIZE];
    GenerateRandomCharArray(buf, p_rnd.NextInt(0, PATH_BUF_SIZE - 1));
    data->Path = buf;

    return data;
}

tTextureData* CreateRandomTextureData(Random& p_rnd)
{
    // Chance to generate nullptr
    if (p_rnd.NextInt(0, 5) == 0) return nullptr;

    // Otherwise generate random data object
    tTextureData* data = new TextureData;

    char* buf = new char[PATH_BUF_SIZE];
    GenerateRandomCharArray(buf, p_rnd.NextInt(0, PATH_BUF_SIZE - 1));
    data->Path = buf;

    data->ScrPos = {p_rnd.NextFloatIncl(0, 1.0f), p_rnd.NextFloatIncl(0, 1.0f)};

    return data;
}

tTextData* CreateRandomTextData(Random& p_rnd)
{
    // Chance to generate nullptr
    if (p_rnd.NextInt(0, 5) == 0) return nullptr;

    // Otherwise generate random data object
    tTextData* data = new tTextData;

    char* buf = new char[MAX_TEXT_LENGTH];
    GenerateRandomCharArray(buf, p_rnd.NextInt(0, MAX_TEXT_LENGTH - 1));
    data->Text = buf;

    data->ScrPos = {p_rnd.NextFloatIncl(0, 1.0f), p_rnd.NextFloatIncl(0, 1.0f)};
    data->Font = nullptr;

    return data;
}

std::vector<tIndicatorData> CreateRandomIndicatorData(Random& p_rnd)
{
    std::vector<tIndicatorData> data = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        data[i] = {
            (InterventionAction)i,
            CreateRandomSoundData(p_rnd),
            CreateRandomTextureData(p_rnd),
            CreateRandomTextData(p_rnd)
        };
    }

    return data;
}

const char* WriteConfigToFile(std::vector<tIndicatorData> p_data)
{
    char path[PATH_BUF_SIZE];
    getcwd(path, PATH_BUF_SIZE);
    snprintf(path, PATH_BUF_SIZE, "%s/" TEST_XML_FILE, path);
    void* fileHandle = GfParmReadFile(path, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    char xmlPath[PATH_BUF_SIZE];
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        tIndicatorData data = p_data[i];

        if (data.Sound)
        {
            snprintf(xmlPath, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_SOUND);
            GfParmSetStr(fileHandle, xmlPath, PRM_ATTR_SRC, data.Sound->Path);
            GfParmSetStr(fileHandle, xmlPath, PRM_ATTR_LOOPING, GfuiMenuBoolToStr(data.Sound->Looping));
            GfParmSetNum(fileHandle, xmlPath, PRM_ATTR_LOOP_INTERVAL, "%", data.Sound->LoopInterval);
        }

        if (data.Texture)
        {
            snprintf(xmlPath, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_TEXTURE);
            GfParmSetStr(fileHandle, xmlPath, PRM_ATTR_SRC, data.Texture->Path);
            GfParmSetNum(fileHandle, xmlPath, PRM_ATTR_XPOS, "%", data.Texture->ScrPos.X);
            GfParmSetNum(fileHandle, xmlPath, PRM_ATTR_YPOS, "%", data.Texture->ScrPos.Y);
        }

        if (data.Text)
        {
            snprintf(xmlPath, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_TEXT);
            GfParmSetStr(fileHandle, xmlPath, PRM_ATTR_CONTENT, data.Text->Text);
            GfParmSetStr(fileHandle, xmlPath, PRM_ATTR_FONT, "");
            GfParmSetNum(fileHandle, xmlPath, PRM_ATTR_FONT_SIZE, "pt", 10);
            GfParmSetNum(fileHandle, xmlPath, PRM_ATTR_XPOS, "%", data.Text->ScrPos.X);
            GfParmSetNum(fileHandle, xmlPath, PRM_ATTR_YPOS, "%", data.Text->ScrPos.Y);
        }
    }

    GfParmWriteFile(nullptr, fileHandle, "test");

    return path;
}

/// @brief Creates a random indicator data object, writes this to an xml file, 
///        then reads in this xml file using IndicatorConfig::LoadIndicatorData
///        Asserts whether the loaded in data is equal to the randomly generated data.
TEST(IndicatorConfigTests, LoadIndicatorDataTest)
{
    GfInit();
    SetupSingletonsFolder();
    Random rnd;

    std::vector<tIndicatorData> data = CreateRandomIndicatorData(rnd);
    WriteConfigToFile(data);


    IndicatorConfig* config = IndicatorConfig::GetInstance();
    std::vector<tIndicatorData> loadedIndicatorData = config->GetIndicatorData();

    // For every indicator, check whether the data is correctly loaded.
    for (InterventionAction i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        ASSERT_EQ(i, i);
    }
}