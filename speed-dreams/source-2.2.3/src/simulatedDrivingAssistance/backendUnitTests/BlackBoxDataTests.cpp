#include <gtest/gtest.h>
#include "BlackBoxData.h"
#include "../rppUtils/Random.hpp"

#define RAND_NAME(a, b)                   \
    for (int i = 0; i < (b)-1; i++)       \
    {                                     \
        (a)[i] = (char)random.NextByte(); \
    }                                     \
    (a)[(b)-1] = '\0';

#define RAND_T3D(a)             \
    (a).x = random.NextFloat(); \
    (a).y = random.NextFloat(); \
    (a).z = random.NextFloat();

// For structure elements where the copy is not implemented, no implementation is given for the generation either, not are there any checks related to that element

struct TestSegments
{
    tTrackSeg* nextSegments;
    int nextSegmentsCount;
};

TestSegments GenerateSegments()
{
    Random random;
    TestSegments testSegments;
    testSegments.nextSegmentsCount = random.NextInt(4);
    testSegments.nextSegments = nullptr;  //new tTrackSeg[testSegments.nextSegmentsCount];
    return testSegments;
}

void DestroySegments(TestSegments& p_testSegments)
{
}

tCarElt GenerateCar(TestSegments& p_testSegments)
{
    Random random;
    tCarElt car{};
    // Assign to car.info
    RAND_NAME(car.info.name, MAX_NAME_LEN)
    RAND_NAME(car.info.sname, MAX_NAME_LEN)
    RAND_NAME(car.info.codename, 4)
    RAND_NAME(car.info.teamname, MAX_NAME_LEN)
    RAND_NAME(car.info.carName, MAX_NAME_LEN)
    RAND_NAME(car.info.category, MAX_NAME_LEN)
    car.info.raceNumber = random.NextInt();
    car.info.startRank = random.NextInt();
    car.info.driverType = random.NextInt();
    car.info.networkplayer = random.NextInt();
    car.info.skillLevel = random.NextInt(4);
    for (int i = 0; i < 3; i++)
    {
        car.info.iconColor[i] = random.NextFloat();
    }
    RAND_T3D(car.info.dimension)
    RAND_T3D(car.info.drvPos)
    RAND_T3D(car.info.bonnetPos)
    car.info.tank = random.NextFloat();
    car.info.steerLock = random.NextFloat();
    RAND_T3D(car.info.statGC)
    for (int i = 0; i < 4; i++)
    {
        car.info.wheel[i].rimRadius = random.NextFloat();
        car.info.wheel[i].tireHeight = random.NextFloat();
        car.info.wheel[i].tireWidth = random.NextFloat();
        car.info.wheel[i].brakeDiskRadius = random.NextFloat();
        car.info.wheel[i].wheelRadius = random.NextFloat();
    }
    car.info.visualAttr.exhaustNb = random.NextInt();
    for (int j = 0; j < 1; j++)
    {
        RAND_T3D(car.info.visualAttr.exhaustPos[j])
    }
    car.info.visualAttr.exhaustPower = random.NextFloat();
    RAND_NAME(car.info.masterModel, MAX_NAME_LEN)
    RAND_NAME(car.info.skinName, MAX_NAME_LEN)
    int possibleSkinTargets[] = {RM_CAR_SKIN_TARGET_WHOLE_LIVERY, RM_CAR_SKIN_TARGET_3D_WHEELS, RM_CAR_SKIN_TARGET_INTERIOR,
                                 RM_CAR_SKIN_TARGET_BOARD, RM_CAR_SKIN_TARGET_DRIVER, RM_CAR_SKIN_TARGET_PIT_DOOR};
    car.info.skinTargets = 0;
    for (int i = 0; i < 6; i++)
    {
        car.info.skinTargets |= possibleSkinTargets[i] * random.NextBool();
    }
    return car;
}

void DestroyCar(tCarElt& p_car)
{
}

tSituation GenerateSituation()
{
    Random random;
    tSituation situation{};
    situation.raceInfo.ncars = random.NextInt();
    situation.raceInfo.totLaps = random.NextInt();
    situation.raceInfo.extraLaps = random.NextInt();
    situation.raceInfo.totTime = random.NextFloat();
    int possibleRaceInfoStates[] = {RM_RACE_RUNNING, RM_RACE_FINISHING, RM_RACE_ENDED, RM_RACE_STARTING, RM_RACE_PRESTART, RM_RACE_PAUSED};
    situation.raceInfo.state = possibleRaceInfoStates[random.NextInt(6)];
    situation.raceInfo.type = random.NextInt(3);
    situation.raceInfo.maxDammage = random.NextInt();
    situation.raceInfo.fps = random.NextUInt();
    situation.raceInfo.features = random.NextInt(16);
    situation.deltaTime = random.NextFloat();
    situation.currentTime = random.NextFloat();
    situation.accelTime = random.NextFloat();
    situation.nbPlayers = random.NextInt();
    situation.cars = nullptr;  // COPY NOT IMPLEMENTED
    return situation;
}

void DestroySituation(tSituation& p_situation)
{
    // Empty body as the only ptr is not implemented
}

class BlackBoxDataTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Random random;
        testSegments = GenerateSegments();
        car = GenerateCar(testSegments);
        situation = GenerateSituation();
    }

    TestSegments testSegments;
    tCarElt car;
    tSituation situation;
    unsigned long tickCount;

    void TearDown() override
    {
        DestroySituation(situation);
        DestroyCar(car);
        DestroySegments(testSegments);
    }
};

TEST_F(BlackBoxDataTest, ValueEqualityTest)
{
    BlackBoxData data = BlackBoxData(&car, &situation, tickCount, testSegments.nextSegments, testSegments.nextSegmentsCount);
    EXPECT_EQ(tickCount, data.TickCount);

    EXPECT_EQ(car.info.wheel[0].rimRadius, data.Car.info.wheel[0].rimRadius);
    EXPECT_EQ(car.info.wheel[3].tireWidth, data.Car.info.wheel[3].tireWidth);

    EXPECT_EQ(car.pub.posMat[2][0], data.Car.pub.posMat[2][0]);
    EXPECT_EQ(car.pub.posMat[0][3], data.Car.pub.posMat[0][3]);

    EXPECT_EQ(car.race.commitBestLapTime, data.Car.race.commitBestLapTime);
    EXPECT_EQ(car.race.currentMinSpeedForLap, data.Car.race.currentMinSpeedForLap);

    EXPECT_EQ(car.priv.collision_state.pos[0], data.Car.priv.collision_state.pos[0]);
    delete car.priv.dashboardInstant[7].setup;

    EXPECT_EQ(car.setup.rideHeight[3].desired_value, data.Car.setup.rideHeight[3].desired_value);
    EXPECT_EQ(car.setup.FRLWeightRep.stepsize, data.Car.setup.FRLWeightRep.stepsize);
    EXPECT_EQ(car.setup.arbSpring[0].max, data.Car.setup.arbSpring[0].max);

    EXPECT_EQ(car.pitcmd.fuel, data.Car.pitcmd.fuel);
    EXPECT_EQ(car.pitcmd.tireChange, data.Car.pitcmd.tireChange);

    EXPECT_EQ(situation.accelTime, data.Situation.accelTime);
    EXPECT_EQ(situation.raceInfo.fps, data.Situation.raceInfo.fps);
    EXPECT_EQ(situation.raceInfo.features, data.Situation.raceInfo.features);
}

TEST_F(BlackBoxDataTest, PointerInequalityTest)
{
    Random random;
    TestSegments testSegments = GenerateSegments();
    tCarElt car = GenerateCar(testSegments);
    tSituation situation = GenerateSituation();
    unsigned long tickCount = random.NextInt();
    BlackBoxData data(&car, &situation, tickCount, testSegments.nextSegments, testSegments.nextSegmentsCount);
    EXPECT_NE(&tickCount, &data.TickCount);
}