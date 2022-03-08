#pragma once
#include <cstdint>
#include <random>
#define byte uint8_t
#define ushort uint16_t
#define uint unsigned int

class Random
{
public:

	//returns random float between 0 and 1
	float NextFloat()
	{
		return static_cast<float>(NextUShort()) / 65536;
	}

	byte NextByte()
	{
		if (p_bytes >= 8) RefreshByte();
		return (byteRandom >> (8 * p_bytes++)) & 0xFF;
	}

	byte NextByte(byte max)
	{
		if (p_bytes >= 8) RefreshByte();
		return ((byteRandom >> (8 * p_bytes++)) & 0xFF) % max;
	}

	ushort NextUShort()
	{
		if (p_shorts >= 4) RefreshShort();
		return (shortRandom >> (16 * p_shorts++)) & 0xFFFF;
	}

	ushort NextUShort(ushort max)
	{
		if (p_shorts >= 4) RefreshShort();
		return ((shortRandom >> (16 * p_shorts++)) & 0xFFFF) % max;
	}

	short NextShort()
	{
		if (p_shorts >= 4) RefreshShort();
		return (shortRandom >> (16 * p_shorts++)) & 0xFFFF;
	}

	short NextShort(short max)
	{
		if (p_shorts >= 4) RefreshShort();
		return ((shortRandom >> (16 * p_shorts++)) & 0xFFFF) % max;
	}

	int NextInt()
	{
		if (p_ints >= 2) RefreshInt();
		return (intRandom >> (32 * p_ints++)) & 0xFFFFFFFF;
	}

	int NextInt(int max)
	{
		if (p_ints >= 2) RefreshInt();
		return ((intRandom >> (32 * p_ints++)) & 0xFFFFFFFF) % max;
	}

	uint NextUInt()
	{
		if (p_ints >= 2) RefreshInt();
		return (intRandom >> (32 * p_ints++)) & 0xFFFFFFFF;
	}

	uint NextUInt(uint max)
	{
		if (p_ints >= 2) RefreshInt();
		return ((intRandom >> (32 * p_ints++)) & 0xFFFFFFFF) % max;
	}

	Random()
	{

#ifdef PSEUDORANDOM
		new (&generator) std::mt19937_64(0xFABCD00AABBCC32);
#else
		std::random_device rd;
		new (&generator) std::mt19937_64(rd());
#endif
		byteRandom = dis(generator);
		shortRandom = dis(generator);
		intRandom = dis(generator);
		p_bytes = 0;
		p_shorts = 0;
		p_ints = 0;
	}

private:

	void RefreshByte()
	{
		byteRandom = dis(generator);
		p_bytes = 0;
	}

	void RefreshShort()
	{
		shortRandom = dis(generator);
		p_shorts = 0;
	}

	void RefreshInt()
	{
		intRandom = dis(generator);
		p_ints = 0;
	}

	long byteRandom;
	int p_bytes; // bytes and shorts are promoted to ints when doing ++

	long shortRandom;
	int p_shorts;

	long intRandom;
	int p_ints;


	std::uniform_int_distribution<long> dis;
	std::mt19937_64 generator;
};