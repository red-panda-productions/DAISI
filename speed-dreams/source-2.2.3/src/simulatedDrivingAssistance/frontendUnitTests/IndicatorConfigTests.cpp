#include <gtest/gtest.h>
#include <tgfclient.h>
#include <portability.h>

#include "../rppUtils/RppUtils.hpp"
#include "../rppUtils/Random.hpp"
#include "TestUtils.h"
#include "IndicatorConfig.h"
#include "Mediator.h"

#define NUM_OF_TESTS    100
#define MAX_TEXT_LENGTH 64
#define TEST_XML_FILE   "test.xml"

// Enum to help generating random indicator data
// Enable multiple flags by doing FLAG1 | FLAG2 | FLAG3
// Check enabled flags by evaluating (flag & FLAG1), (flag & FLAG2), etc. as boolean
typedef unsigned int DataGeneration;
#define VALID                 0         // Generates completely valid data
#define CAN_GENERATE_NULL     (1 << 0)  // Generates valid data with a chance to not generate sound/text/texture data
#define INVALID_SCR_POS       (1 << 1)  // Generates invalid screen positions
#define INVALID_LOOP_INTERVAL (1 << 2)  // Generates invalid loop intervals

/// @brief Test Fixture for testing the loading of indicatorconfig data
///        Handles the Setup for every test and logs the seed used to generate random data.
class IndicatorConfigLoadingTests : public ::testing::Test
{
private:
    Random m_rnd;

protected:
    /// @brief Setup Gf module to have access to its XML reading/writing functions
    ///        Creates the random number generator to be used during the test and log its seed.
    void SetUp() override
    {
        GfInit(GF_LOGGING_DISABLE);
        GfSetDataDir(SD_DATADIR_SRC);
        ASSERT_TRUE(SetupSingletonsFolder());
        m_rnd = Random();
        GTEST_COUT << "Random Seed: " << m_rnd.GetSeed() << std::endl;

        // Needs to be on something other than NO_SIGNALS to retrieve active indicators
        SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_ONLY_SIGNALS);
    }

    /// @brief       Creates randomly generated sound data
    /// @param p_gen The flags to use when generating data
    /// @return      The generated sound data
    tSoundData* CreateRandomSoundData(DataGeneration p_gen)
    {
        // Chance to generate nullptr
        if (p_gen & CAN_GENERATE_NULL && SucceedWithChance(m_rnd, 20)) return nullptr;

        // Otherwise generate random data object
        tSoundData* data = new SoundData;
        data->ActiveLastFrame = false;
        data->Looping = m_rnd.NextBool();

        if (p_gen & INVALID_LOOP_INTERVAL)
        {
            data->LoopInterval = m_rnd.NextFloat(-FLT_MAX, 0);
        }
        else
        {
            data->LoopInterval = m_rnd.NextFloat(0, FLT_MAX);
        }

        char* buf = new char[PATH_BUF_SIZE];
        GenerateRandomCharArray(buf, m_rnd.NextInt(0, PATH_BUF_SIZE - 1));
        data->Path = buf;

        return data;
    }

    /// @brief  Creates a random invalid screen coordinate in the range [-FLOAT_MAX..0][1..FLOAT_MAX]
    /// @return The invalid screen coordinate.
    float CreateInvalidScreenCoord()
    {
        return (m_rnd.NextBool()) ? m_rnd.NextFloat(1.0f, FLT_MAX) : m_rnd.NextFloat(-FLT_MAX, 0);
    }

    /// @brief       Creates randomly generated screen positions
    /// @param p_gen The flags to use when generating data
    /// @return      The generated screen position
    tScreenPosition CreateRandomScreenPosition(DataGeneration p_gen)
    {
        if (p_gen & INVALID_SCR_POS)
        {
            return {CreateInvalidScreenCoord(), CreateInvalidScreenCoord()};
        }
        return {m_rnd.NextFloatIncl(0, 1.0f), m_rnd.NextFloatIncl(0, 1.0f)};
    }

    /// @brief       Creates randomly generated texture data
    /// @param p_gen The flags to use when generating data
    /// @return      The generated texture data
    tTextureData* CreateRandomTextureData(DataGeneration p_gen)
    {
        // Chance to generate nullptr
        if (p_gen & CAN_GENERATE_NULL && SucceedWithChance(m_rnd, 20)) return nullptr;

        // Otherwise generate random data object
        tTextureData* data = new TextureData;

        char* buf = new char[PATH_BUF_SIZE];
        GenerateRandomCharArray(buf, m_rnd.NextInt(0, PATH_BUF_SIZE - 1));
        data->Path = buf;
        data->ScrPos = CreateRandomScreenPosition(p_gen);

        return data;
    }

    /// @brief       Creates randomly generated text data
    /// @param p_gen The flags to use when generating data
    /// @return      The generated text data
    tTextData* CreateRandomTextData(DataGeneration p_gen)
    {
        // Chance to generate nullptr
        if (p_gen & CAN_GENERATE_NULL && SucceedWithChance(m_rnd, 20)) return nullptr;

        // Otherwise generate random data object
        tTextData* data = new tTextData;

        char* buf = new char[MAX_TEXT_LENGTH];
        GenerateRandomCharArray(buf, m_rnd.NextInt(0, MAX_TEXT_LENGTH - 1));
        data->Text = buf;

        data->ScrPos = CreateRandomScreenPosition(p_gen);

        // Cannot randomly create a font class, this would require to only store the path and font-size
        // in the indicator config and to load them in later, then we would be able to generate a random
        // path and font-size here. However, currently the IndicatorConfig immediatly loads the Font object,
        // which has no font name and/or members for font size.
        data->Font = nullptr;

        return data;
    }

    /// @brief       Creates a vector of randomly generated indicator data
    /// @param p_gen The flags to use when generating data
    /// @return      The vector of generated indicator data
    std::vector<tIndicatorData> CreateRandomIndicatorData(DataGeneration p_gen)
    {
        //std::vector<tIndicatorData> data = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);
        std::vector<tIndicatorData> data = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);
        for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
        {
            data[i] = {
                (InterventionAction)i,
                CreateRandomSoundData(p_gen),
                CreateRandomTextureData(p_gen),
                CreateRandomTextData(p_gen)};
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
                GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_XPOS, nullptr, data.Text->ScrPos.X);
                GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_YPOS, nullptr, data.Text->ScrPos.Y);

                // Write a valid font to not have errors when loading, but isn't actually asserted later
                // See the comment in CreateRandomTextData for more info.
                GfParmSetStr(fileHandle, xmlSection, PRM_ATTR_FONT, "b5.glf");
                GfParmSetNum(fileHandle, xmlSection, PRM_ATTR_FONT_SIZE, nullptr, 10);
            }
        }

        GfParmWriteFile(nullptr, fileHandle, "test");

        return path;
    }

    /// @brief             Asserts whether the loaded sound is equal to the generated random sound.
    /// @param p_loadedSnd The sound loaded into the indicator config
    /// @param p_rndSnd    The randomly generated sound
    void AssertSound(tSoundData* p_loadedSnd, tSoundData* p_rndSnd)
    {
        // If the generated sound is null, only check whether the loaded sound is also null
        if (!p_rndSnd)
        {
            ASSERT_EQ(p_loadedSnd, p_rndSnd);
            return;
        }

        char* rndSndFullPath = new char[PATH_BUF_SIZE];
        snprintf(rndSndFullPath, PATH_BUF_SIZE, SOUNDS_DIR_FORMAT, GfDataDir(), p_rndSnd->Path);
        ASSERT_STREQ(p_loadedSnd->Path, rndSndFullPath);
        ASSERT_EQ(p_loadedSnd->Looping, p_rndSnd->Looping);
        ASSERT_EQ(p_loadedSnd->LoopInterval, p_rndSnd->LoopInterval);
    }

    /// @brief             Asserts whether the loaded texture is equal to the generated random texture.
    /// @param p_loadedTex The texture loaded into the indicator config
    /// @param p_rndTex    The randomly generated texture
    void AssertTexture(tTextureData* p_loadedTex, tTextureData* p_rndTex)
    {
        // If the generated texture is null, only check whether the loaded texture is also null
        if (!p_rndTex)
        {
            ASSERT_EQ(p_loadedTex, p_rndTex);
            return;
        }

        ASSERT_STREQ(p_loadedTex->Path, p_rndTex->Path);
        ASSERT_EQ(p_loadedTex->ScrPos.X, p_rndTex->ScrPos.X);
        ASSERT_EQ(p_loadedTex->ScrPos.Y, p_rndTex->ScrPos.Y);
    }

    /// @brief             Asserts whether the loaded text is equal to the generated random text.
    /// @param p_loadedTxt The text loaded into the indicator config
    /// @param p_rndTxt    The randomly generated text
    void AssertText(tTextData* p_loadedTxt, tTextData* p_rndTxt)
    {
        // If the generated texture is null, only check whether the loaded texture is also null
        if (!p_rndTxt)
        {
            ASSERT_EQ(p_loadedTxt, p_rndTxt);
            return;
        }

        ASSERT_STREQ(p_loadedTxt->Text, p_rndTxt->Text);
        ASSERT_EQ(p_loadedTxt->ScrPos.X, p_rndTxt->ScrPos.X);
        ASSERT_EQ(p_loadedTxt->ScrPos.Y, p_rndTxt->ScrPos.Y);
    }

    /// @brief                    Asserts whether the loaded indicator is equal to the generated indicator
    /// @param p_loadedIndicator  The indicator loaded into the indicator config
    /// @param p_rndIndicator     The randomly generated indicator
    void AssertIndicator(tIndicatorData p_loadedIndicator, tIndicatorData p_rndIndicator)
    {
        ASSERT_EQ(p_loadedIndicator.Action, p_rndIndicator.Action);
        AssertSound(p_loadedIndicator.Sound, p_rndIndicator.Sound);
        AssertTexture(p_loadedIndicator.Texture, p_rndIndicator.Texture);
        AssertText(p_loadedIndicator.Text, p_rndIndicator.Text);
    }
};

/// @brief Creates a random indicator data object, writes this to an xml file,
///        then reads in this xml file using IndicatorConfig::LoadIndicatorData
///        Asserts whether the loaded in data is equal to the randomly generated data.
TEST_F(IndicatorConfigLoadingTests, LoadIndicatorDataFromXML)
{
    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random valid indicator data with a chance to generate no sound/texture/text data
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(CAN_GENERATE_NULL);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        // Load the created xml file into the indicator config and
        // test whether every value matches the original generated value.
        IndicatorConfig::GetInstance()->LoadIndicatorData(filepath);
        std::vector<tIndicatorData> loadedData = IndicatorConfig::GetInstance()->GetIndicatorData();
        for (InterventionAction i = 0; i < NUM_INTERVENTION_ACTION; i++)
        {
            AssertIndicator(loadedData[i], rndData[i]);
        }
    }
}

/// @brief Tests whether the IndicatorConfig correctly throws an error for invalid screen positions
TEST_F(IndicatorConfigLoadingTests, ThrowExceptionInvalidScreenPosition)
{
    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random indicator data with invalid screen positions and write it to xml
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(INVALID_SCR_POS);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        ASSERT_THROW(IndicatorConfig::GetInstance()->LoadIndicatorData(filepath), std::out_of_range);
    }
}

/// @brief Tests whether the IndicatorConfig correctly throws an error for invalid loop intervals
TEST_F(IndicatorConfigLoadingTests, ThrowExceptionInvalidLoopInterval)
{
    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random indicator data with invalid loop intervals and write it to xml
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(INVALID_LOOP_INTERVAL);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        ASSERT_THROW(IndicatorConfig::GetInstance()->LoadIndicatorData(filepath), std::runtime_error);
    }
}

/// @brief Tests whether the IndicatorConfig can activate an indicator and retrieve the correct values.
TEST_F(IndicatorConfigLoadingTests, ActivateIndicator)
{
    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
        // Create random valid indicator data
        std::vector<tIndicatorData> rndData = CreateRandomIndicatorData(VALID);
        const char* filepath = WriteIndicatorDataToXml(rndData);

        // Activate every action and check whether the corresponding action is also returned by GetActiveIndicators.
        IndicatorConfig::GetInstance()->LoadIndicatorData(filepath);
        for (InterventionAction i = 0; i < NUM_INTERVENTION_ACTION; i++)
        {
            IndicatorConfig::GetInstance()->ActivateIndicator(i);
            std::vector<tIndicatorData> active = IndicatorConfig::GetInstance()->GetActiveIndicators(INTERVENTION_TYPE_ONLY_SIGNALS);

            // Currently there is only 1 indicator active at the time, so we can just retrieve it with [0].
            // TODO: update whenever the IndicatorConfig can have multiple indicators active at a time.
            AssertIndicator(active[0], rndData[i]);
        }
    }
}

/// @brief Test Fixture for testing the indicatorconfig singleton properties
class IndicatorConfigSingletonTests : public ::testing::Test
{
protected:
    /// @brief Ensures that there is a clean version of the IndicatorConfig to run tests on.
    void SetUp() override
    {
        IndicatorConfig::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
    }
};

/// @brief Tests whether two configs point towards the same instance.
TEST_F(IndicatorConfigSingletonTests, SingletonInstance)
{
    IndicatorConfig* config1 = IndicatorConfig::GetInstance();
    IndicatorConfig* config2 = IndicatorConfig::GetInstance();
    ASSERT_EQ(config1, config2);
}

/// @brief Tests whether two configs from different modules point to the same instance.
///        This requires the instance pointer to be stored and read from a file.
TEST_F(IndicatorConfigSingletonTests, SingletonInstanceFromFile)
{
    // Create first instance, which also stores it into the file.
    IndicatorConfig* config1 = IndicatorConfig::GetInstance();

    // By clearing the instance but not the singletons folder, it forces the next instance to be retreived from the file.
    IndicatorConfig::ClearInstance();
    IndicatorConfig* config2 = IndicatorConfig::GetInstance();
    ASSERT_EQ(config1, config2);
}