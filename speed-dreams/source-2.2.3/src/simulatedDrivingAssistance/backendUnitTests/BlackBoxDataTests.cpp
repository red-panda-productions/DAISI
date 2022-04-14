#include <gtest/gtest.h>
#include "BlackBoxData.h"
#include "../rppUtils/Random.hpp"

TEST(BlackBoxDataTests,ValueEqualityTest)
{
	Random random;
	tCarElt car;
	tSituation situation;
	int tickCount = random.NextInt();
	// Assign values to car
	car.info.wheel[0].rimRadius = random.NextFloat();
	car.info.wheel[3].tireWidth = random.NextFloat();
	car.pub.posMat[2][0] = random.NextFloat();
	car.pub.posMat[0][3] = random.NextFloat();
	car.race.commitBestLapTime = random.NextBool();
	car.race.currentMinSpeedForLap = random.NextFloat();
	car.priv.dashboardInstant[7].setup = new tCarSetupItem();
	car.priv.collision_state.pos[0] = random.NextFloat();
	car.setup.rideHeight[3].desired_value = random.NextFloat();
	car.setup.FRLWeightRep.stepsize = random.NextInt();
	car.setup.arbSpring[0].max = random.NextFloat();
	car.pitcmd.fuel = random.NextFloat();
	tCarPitCmd::TireChange tireChange[2] = { tCarPitCmd::NONE, tCarPitCmd::ALL };
	car.pitcmd.tireChange = tireChange[random.NextInt(2)];
	// Assign values to situation
	situation.accelTime = random.NextFloat();
	situation.raceInfo.fps = random.NextUInt();
	situation.raceInfo.features = random.NextInt();
	BlackBoxData data = BlackBoxData(&car, &situation, tickCount);
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

TEST(BlackBoxDataTests,PointerInequalityTest)
{
	Random random;
	tCarElt car;
	tSituation situation;
	int tickCount = random.NextInt();
	// Assign values to car
	// Assign values to situation
	BlackBoxData data = BlackBoxData(&car, &situation, tickCount);
	EXPECT_NE(&tickCount, &data.TickCount);
}