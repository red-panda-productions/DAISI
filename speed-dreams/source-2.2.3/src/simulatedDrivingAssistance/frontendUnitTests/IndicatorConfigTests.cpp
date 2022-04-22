#include <gtest/gtest.h>
#include <tgfclient.h>
#include <portability.h>

#include "../rppUtils/RppUtils.hpp"
#include "../rppUtils/Random.hpp"
#include "TestUtils.h"
#include "IndicatorConfig.h"


#define NUM_OF_TESTS      100
#define MAX_TEXT_LENGTH   64
#define TEST_XML_FILE     "test.xml"

// Enum to help generating random indicator data
// Enable multiple flags by doing FLAG1 | FLAG2 | FLAG3
// Extract enabled flags by checking (flag & FLAG1), (flag & FLAG2), etc
typedef unsigned int DataValidity;
#define VALID                 0
#define INVALID_SCR_POS       (1 << 0)
#define INVALID_LOOP_INTERVAL (1 << 1)


/// @brief            Creates randomly generated sound data
/// @param p_rnd      The random generator reference to use
/// @param p_validity The validity flags to use when generating data
/// @return           The generated sound data
tSoundData* CreateRandomSoundData(Random& p_rnd, DataValidity p_validity)
{
    // Chance to generate nullptr
    if (p_rnd.NextInt(0, 5) == 0) return nullptr;

    // Otherwise generate random data object
    tSoundData* data = new SoundData;
    data->ActiveLastFrame = false;
    data->Looping = p_rnd.NextBool();

    if (p_validity & INVALID_LOOP_INTERVAL)
    {
        data->LoopInterval = p_rnd.NextFloat(-FLT_MAX, 0);
    }
    else
    {
        data->LoopInterval = p_rnd.NextFloat(0, FLT_MAX);
    }

    char* buf = new char[PATH_BUF_SIZE];
    GenerateRandomCharArray(buf, p_rnd.NextInt(0, PATH_BUF_SIZE - 1));
    data->Path = buf;

    return data;
}

/// @brief            Creates randomly generated screen positions
/// @param p_rnd      The random generator reference to use
/// @param p_validity The validity flags to use when generating data
/// @return           The generated screen position      
tScreenPosition CreateRandomScreenPosition(Random& p_rnd, DataValidity p_validity)
{
    if (p_validity & INVALID_SCR_POS)
    {
        return {p_rnd.NextFloat(1.0f, FLT_MAX), p_rnd.NextFloat(1.0f, FLT_MAX)};
    }
    return {p_rnd.NextFloatIncl(0, 1.0f), p_rnd.NextFloatIncl(0, 1.0f)};
}

/// @brief            Creates randomly generated texture data
/// @param p_rnd      The random generator reference to use
/// @param p_validity The validity flags to use when generating data
/// @return           The generated texture data
tTextureData* CreateRandomTextureData(Random& p_rnd, DataValidity p_validity)
{
    // Chance to generate nullptr
    if (p_rnd.NextInt(0, 5) == 0) return nullptr;

    // Otherwise generate random data object
    tTextureData* data = new TextureData;

    char* buf = new char[PATH_BUF_SIZE];
    GenerateRandomCharArray(buf, p_rnd.NextInt(0, PATH_BUF_SIZE - 1));
    data->Path = buf;
    data->ScrPos = CreateRandomScreenPosition(p_rnd, p_validity);

    return data;
}

/// @brief            Creates randomly generated text data
/// @param p_rnd      The random generator reference to use
/// @param p_validity The validity flags to use when generating data
/// @return           The generated text data
tTextData* CreateRandomTextData(Random& p_rnd, DataValidity p_validity)
{
    // Chance to generate nullptr
    if (p_rnd.NextInt(0, 5) == 0) return nullptr;

    // Otherwise generate random data object
    tTextData* data = new tTextData;

    char* buf = new char[MAX_TEXT_LENGTH];
    GenerateRandomCharArray(buf, p_rnd.NextInt(0, MAX_TEXT_LENGTH - 1));
    data->Text = buf;

    data->ScrPos = CreateRandomScreenPosition(p_rnd, p_validity);

    // Cannot randomly create a font class, this would require to only store the path and font-size
    // in the indicator config and to load them in later, then we would be able to generate a random 
    // path and font-size here.
    data->Font = nullptr;

    return data;
}

/// @brief            Creates a vector of randomly generated indicator data
/// @param p_rnd      The random generator reference to use
/// @param p_validity The validity flags to use when generating data
/// @return           The vector of generated indicator data
std::vector<tIndicatorData> CreateRandomIndicatorData(Random& p_rnd, DataValidity p_validity)
{
    std::vector<tIndicatorData> data = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        data[i] = {
            (InterventionAction)i,
            CreateRandomSoundData(p_rnd, p_validity),
            CreateRandomTextureData(p_rnd, p_validity),
            CreateRandomTextData(p_rnd, p_validity)
        };
    }

    return data;
}

/// @brief        Writes the given indicator data object to a xml file.
/// @param p_data The indicator data
/// @return       The filepath of the xml file that it has been written to.
const char* WriteIndicatorDataToXml(std::vector<tIndicatorData> p_data)
{
    // Open or create the xml file to write to, clean the previous parameters.
    char* path = new char[PATH_BUF_SIZE];
    getcwd(path, PATH_BUF_SIZE);
    snprintf(path, PATH_BUF_SIZE, "%s/" TEST_XML_FILE, path);
    void* fileHandle = GfParmReadFile(path, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);
    GfParmClean(fileHandle);

    // Write all data to the xml file.
    char xmlSection[PATH_BUF_SIZE];
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        tIndicatorData data = p_data[i];

        if (data.Sound)
        {
            snprintf(xmlSection, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_SOUND);
            GfParmSetStr(fileHandle, xmlSection, PRM_ATTR_SRC, data.Sound->Path);
            GfParmSetStr(fileHandle, xmlSection, PRM_ATTR_LOOPING, GfuiMenuBoolToStr(data.Sound->Looping));
            GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_LOOP_INTERVAL, nullptr, data.Sound->LoopInterval);
        }

        if (data.Texture)
        {
            snprintf(xmlSection, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_TEXTURE);
            GfParmSetStr(fileHandle, xmlSection, PRM_ATTR_SRC, data.Texture->Path);
            GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_XPOS, nullptr, data.Texture->ScrPos.X);
            GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_YPOS, nullptr, data.Texture->ScrPos.Y);
        }

        if (data.Text)
        {
            snprintf(xmlSection, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_TEXT);
            GfParmSetStr(fileHandle, xmlSection, PRM_ATTR_CONTENT, data.Text->Text);
            GfParmSetStr(fileHandle, xmlSection, PRM_ATTR_FONT, nullptr);
            GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_FONT_SIZE, nullptr, 10);
            GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_XPOS, nullptr, data.Text->ScrPos.X);
            GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_YPOS, nullptr, data.Text->ScrPos.Y);
        }
    }

    GfParmWriteFile(nullptr, fileHandle, "test");

    return path;
}

/// @brief           Asserts whether the loaded sound is equal to the generated random sound.
/// @param loadedTxt The sound loaded into the indicator config
/// @param rndTxt    The randomly generated sound.
void AssertSound(tSoundData* loadedSnd, tSoundData* rndSnd)
{
    // If the generated sound is null, only check whether the loaded sound is also null
    if (!rndSnd)
    {
        ASSERT_EQ(loadedSnd, rndSnd);
        return;
    }

    char* rndSndFullPath = new char[PATH_BUF_SIZE];
    snprintf(rndSndFullPath, PATH_BUF_SIZE, SOUNDS_DIR_FORMAT, GfDataDir(), rndSnd->Path);
    ASSERT_STREQ(loadedSnd->Path, rndSndFullPath);
    ASSERT_EQ(loadedSnd->Looping, rndSnd->Looping);
    ASSERT_EQ(loadedSnd->LoopInterval, rndSnd->LoopInterval);
}

/// @brief           Asserts whether the loaded texture is equal to the generated random texture.
/// @param loadedTxt The texture loaded into the indicator config
/// @param rndTxt    The randomly generated texture.
void AssertTexture(tTextureData* loadedTex, tTextureData* rndTex)
{
    // If the generated texture is null, only check whether the loaded texture is also null
    if (!rndTex)
    {
        ASSERT_EQ(loadedTex, rndTex);
        return;
    }

    ASSERT_STREQ(loadedTex->Path, rndTex->Path);
    ASSERT_EQ(loadedTex->ScrPos.X, rndTex->ScrPos.X);
    ASSERT_EQ(loadedTex->ScrPos.Y, rndTex->ScrPos.Y);
}

/// @brief           Asserts whether the loaded text is equal to the generated random text.
/// @param loadedTxt The text loaded into the indicator config
/// @param rndTxt    The randomly generated text.
void AssertText(tTextData* loadedTxt, tTextData* rndTxt)
{
    // If the generated texture is null, only check whether the loaded texture is also null
    if (!rndTxt)
    {
        ASSERT_EQ(loadedTxt, rndTxt);
        return;
    }

    ASSERT_STREQ(loadedTxt->Text, rndTxt->Text);
    ASSERT_EQ(loadedTxt->ScrPos.X, rndTxt->ScrPos.X);
    ASSERT_EQ(loadedTxt->ScrPos.Y, rndTxt->ScrPos.Y);
}

/// @brief Creates a random indicator data object, writes this to an xml file, 
///        then reads in this xml file using IndicatorConfig::LoadIndicatorData
///        Asserts whether the loaded in data is equal to the randomly generated data.
TEST(IndicatorConfigTests, LoadIndicatorDataTest)
{
    // Gf module contains all the XML reading/writing functions.
    GfInit();
    ASSERT_TRUE(SetupSingletonsFolder());
    Random rnd;

    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random valid indicator data and write it to xml
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(rnd, VALID);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        // Load the created xml file into the indicator config and
        // test whether every value matches the original generated value.
        IndicatorConfig::GetInstance()->LoadIndicatorData(filepath);
        std::vector<tIndicatorData> loadedData = IndicatorConfig::GetInstance()->GetIndicatorData();
        for (InterventionAction i = 0; i < NUM_INTERVENTION_ACTION; i++)
        {
            ASSERT_EQ(loadedData[i].Action, rndData[i].Action);

            AssertSound(loadedData[i].Sound, rndData[i].Sound);
            AssertTexture(loadedData[i].Texture, rndData[i].Texture);
            AssertText(loadedData[i].Text, rndData[i].Text);
        }
    }
}

/// @brief Tests whether two configs point towards the same instance. 
TEST(IndicatorConfigTests, IsSingletonIndicatorConfig)
{
    ASSERT_TRUE(SetupSingletonsFolder());
    IndicatorConfig* config1 = IndicatorConfig::GetInstance();
    IndicatorConfig* config2 = IndicatorConfig::GetInstance();
    ASSERT_EQ(config1, config2);
}

/// @brief Tests whether the IndicatorConfig correctly throws an error for invalid screen positions
TEST(IndicatorConfigTests, ThrowExceptionInvalidScreenPosition)
{
    // Gf module contains all the XML reading/writing functions.
    GfInit();
    ASSERT_TRUE(SetupSingletonsFolder());
    Random rnd;

    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random indicator data with invalid screen positions and write it to xml
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(rnd, INVALID_SCR_POS);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        ASSERT_THROW(IndicatorConfig::GetInstance()->LoadIndicatorData(filepath), std::out_of_range);
    }
}

/// @brief Tests whether the IndicatorConfig correctly throws an error for invalid loop intervals 
TEST(IndicatorConfigTests, ThrowExceptionInvalidLoopInterval)
{
    // Gf module contains all the XML reading/writing functions.
    GfInit();
    ASSERT_TRUE(SetupSingletonsFolder());
    Random rnd;

    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random indicator data with invalid loop intervals and write it to xml
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(rnd, INVALID_LOOP_INTERVAL);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        ASSERT_THROW(IndicatorConfig::GetInstance()->LoadIndicatorData(filepath), std::runtime_error);
    }
}