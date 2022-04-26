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

#define MockMediator Mediator<DecisionMakerMock>

template <>
MockMediator* MockMediator::m_instance = nullptr;

/// @brief Test if the distribution of the mediator works
TEST(MediatorTest, GetDistributedMediatorTemplated)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator* mediator1 = MockMediator::GetInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
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
    struct stat info;
    std::experimental::filesystem::path path = std::experimental::filesystem::temp_directory_path();
    path.append("Singletons\\Mediator");
    std::string pathstring = path.string();
    const char* filepath = pathstring.c_str();
    int err = stat(filepath, &info);
    if (err != -1) return false;
    std::ofstream file(filepath);
    file << p_mediator;
    file.close();
    return true;
}

/// @brief Tests if reading a mediator pointer from a file works
TEST(MediatorTest, ReadFromFile)
{
    SMediator* fakeMediator = new SMediator();
    EXPECT_TRUE(WriteMediator(fakeMediator));
    SMediator* mediator = SMediator::GetInstance();
    EXPECT_EQ(fakeMediator, mediator);
    delete fakeMediator;
}