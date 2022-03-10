#pragma once
#include <cstdint>
#include <random>
#define byte uint8_t
#define ushort uint16_t
#define uint unsigned int

class Random
{
public:
	float NextFloat()
	{
		return static_cast<float>(NextUShort()) / 65536.0f;
	}

	float NextFloat(float max)
	{
		return static_cast<float>(NextUShort()) / (65536.0f / max);
	}

	float NextFloat(float min, float max)
	{
		return NextFloat(max - min) + min;
	}

	byte NextByte()
	{
		if (m_bytes >= 8) RefreshByte();
		return (m_byteRandom >> (8 * m_bytes++)) & 0xFF;
	}

	byte NextByte(byte max)
	{
		if (m_bytes >= 8) RefreshByte();
		return ((m_byteRandom >> (8 * m_bytes++)) & 0xFF) % max;
	}

	byte NextByte(byte min, byte max)
	{
		return NextByte(max - min) + min;
	}

	ushort NextUShort()
	{
		if (m_shorts >= 4) RefreshShort();
		return (m_shortRandom >> (16 * m_shorts++)) & 0xFFFF;
	}

	ushort NextUShort(ushort max)
	{
		if (m_shorts >= 4) RefreshShort();
		return ((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF) % max;
	}

	ushort NextUShort(ushort min, ushort max)
	{
		return NextUShort(max - min) + min;
	}

	short NextShort()
	{
		if (m_shorts >= 4) RefreshShort();
		return (m_shortRandom >> (16 * m_shorts++)) & 0xFFFF;
	}

	short NextShort(short max)
	{
		if (m_shorts >= 4) RefreshShort();
		return ((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF) % max;
	}

	short NextShort(short min, short max)
	{
		return NextShort(max - min) + min;
	}

	int NextInt()
	{
		if (m_ints >= 2) RefreshInt();
		return (m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF;
	}

	int NextInt(int max)
	{
		if (m_ints >= 2) RefreshInt();
		return ((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF) % max;
	}

	int NextInt(int min, int max)
	{
		return NextInt(max - min) + min;
	}

	uint NextUInt()
	{
		if (m_ints >= 2) RefreshInt();
		return (m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF;
	}

	uint NextUInt(uint max)
	{
		if (m_ints >= 2) RefreshInt();
		return ((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF) % max;
	}

	uint NextUint(uint min, uint max)
	{
		return NextUInt(max - min) + min;
	}

	bool NextBool()
	{
		if (m_bools >= 64) RefreshBool();
		return (m_boolRandom & (1UL << m_bools++)) > 0;
	}

	Random()
	{

#ifdef PSEUDORANDOM
		new (&generator) std::mt19937_64(0xFABCD00AABBCC32); //just a random chosen number
#else
		std::random_device rd;
		new (&m_generator) std::mt19937_64(rd());
#endif
		m_byteRandom = m_dis(m_generator);
		m_shortRandom = m_dis(m_generator);
		m_intRandom = m_dis(m_generator);
		m_boolRandom = m_dis(m_generator);
		m_bytes = 0;
		m_shorts = 0;
		m_ints = 0;
	}

private:

	void RefreshByte()
	{
		m_byteRandom = m_dis(m_generator);
		m_bytes = 0;
	}

	void RefreshShort()
	{
		m_shortRandom = m_dis(m_generator);
		m_shorts = 0;
	}

	void RefreshInt()
	{
		m_intRandom = m_dis(m_generator);
		m_ints = 0;
	}

	void RefreshBool()
	{
		m_boolRandom = m_dis(m_generator);
		m_bools = 0;
	}

	long m_byteRandom;
	int m_bytes; // bytes and shorts are promoted to ints when doing ++

	long m_shortRandom;
	int m_shorts;

	long m_intRandom;
	int m_ints;

	unsigned long m_boolRandom;
	int m_bools;


	std::uniform_int_distribution<long> m_dis;
	std::mt19937_64 m_generator;
};