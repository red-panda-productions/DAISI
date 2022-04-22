#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "mocks/DecisionMakerMock.h"
#include <sys/types.h>
#include <sys/stat.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
#include "../rppUtils/RppUtils.hpp"

#define TMediator Mediator<DecisionMakerMock>

template <>
TMediator* TMediator::m_instance = nullptr;

/// @brief Test if the distribution of the mediator works
TEST(MediatorTest, GetDistributedMediatorTemplated)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    TMediator* mediator1 = TMediator::GetInstance();
    TMediator* mediator2 = TMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
}

/// @brief Test if the distribution of the mediator works
TEST(MediatorTest, GetDistributedMediator)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator1 = SMediator::GetInstance();
    SMediator* mediator2 = SMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
}

/// @brief                    Tests if the Mediator sets and gets the interventionType correctly
/// @param p_interventionType The interventionType that needs to be set
void InterventionTest(InterventionType p_interventionType)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator = SMediator::GetInstance();

    mediator->SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator->GetInterventionType());
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTest, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTest, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTest, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))

/// @brief            Writes a given mediator pointer to the Singletons/Mediator file
/// @param p_mediator The mediator pointer to write to the file
bool WriteMediator(SMediator* p_mediator)
{
    EXPECT_TRUE(SetupSingletonsFolder());
    struct stat info
    {
    };
    std::experimental::filesystem::path path = std::experimental::filesystem::temp_directory_path();
    path.append("Singletons\\Mediator");
    std::string pathstring = path.string();
    const char* filepath = pathstring.c_str();
    int err = stat(filepath, &info);
    if (err == -1)
    {
        std::ofstream file(filepath);
        file << p_mediator;
        file.close();
        return true;
    }
    return false;  // If we get here, SetupSingletonsFolder() already failed
}

TEST(MediatorTest, ReadFromFile)
{
    SMediator::GetInstance();
    SMediator* oldMediator = SMediator::RemoveInstance();  // Makes sure we can't get into the if (m_instance) block
    SMediator* fakeMediator = (SMediator*)malloc(sizeof (SMediator));
    EXPECT_TRUE(WriteMediator(fakeMediator));
    SMediator* mediator = SMediator::GetInstance();
    EXPECT_EQ(fakeMediator, mediator);

    // Return SMediator to its old value
    EXPECT_TRUE(WriteMediator(oldMediator));
    SMediator::RemoveInstance();
    SMediator* newMediator = SMediator::GetInstance();
    EXPECT_EQ(newMediator, oldMediator);

    free(fakeMediator);
}