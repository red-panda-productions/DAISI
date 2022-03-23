#pragma once
#include <cstdint>
#include <random>
#define byte uint8_t
#define ushort uint16_t
#define uint unsigned int

/// @brief Creates random numbers
class Random
{
public:

	/// @return A random float
	float NextFloat()
	{
		return static_cast<float>(NextUShort()) / 65536.0f;
	}

	/// @return A random float with a maximum
	float NextFloat(float p_max)
	{
		return static_cast<float>(NextUShort()) / (65536.0f / p_max);
	}

	/// @return A random float between a max and a min
	float NextFloat(float p_min, float p_max)
	{
		return NextFloat(p_max - p_min) + p_min;
	}

	/// @return A random byte
	byte NextByte()
	{
		if (m_bytes >= 8) RefreshByte();
		return (m_byteRandom >> (8 * m_bytes++)) & 0xFF;
	}

	/// @return A random byte with a maximum
	byte NextByte(byte p_max)
	{
		if (m_bytes >= 8) RefreshByte();
		return ((m_byteRandom >> (8 * m_bytes++)) & 0xFF) % p_max;
	}

	/// @return A random byte between a maximum and a minimum
	byte NextByte(byte p_min, byte p_max)
	{
		return NextByte(p_max - p_min) + p_min;
	}

	/// @return A random ushort
	ushort NextUShort()
	{
		if (m_shorts >= 4) RefreshShort();
		return (m_shortRandom >> (16 * m_shorts++)) & 0xFFFF;
	}

	/// @return A random ushort with a maximum
	ushort NextUShort(ushort p_max)
	{
		if (m_shorts >= 4) RefreshShort();
		return ((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF) % p_max;
	}

	/// @return A random ushort between a maximum and a minimum
	ushort NextUShort(ushort p_min, ushort p_max)
	{
		return NextUShort(p_max - p_min) + p_min;
	}

	/// @return A random short
	short NextShort()
	{
		if (m_shorts >= 4) RefreshShort();
		return (m_shortRandom >> (16 * m_shorts++)) & 0xFFFF;
	}

	/// @return A random short with a maximum
	short NextShort(short p_max)
	{
		if (m_shorts >= 4) RefreshShort();
		return ((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF) % p_max;
	}

	/// @return A random short between a maximum and a minimum
	short NextShort(short p_min, short p_max)
	{
		return NextShort(p_max - p_min) + p_min;
	}

	/// @return A random int
	int NextInt()
	{
		if (m_ints >= 2) RefreshInt();
		return (m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF;
	}

	/// @return A random int with a maximum
	int NextInt(int p_max)
	{
		if (m_ints >= 2) RefreshInt();
		return ((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF) % p_max;
	}

	/// @return A random int between a maximum and a minimum
	int NextInt(int p_min, int p_max)
	{
		return NextInt(p_max - p_min) + p_min;
	}

	/// @return A random uint
	uint NextUInt()
	{
		if (m_ints >= 2) RefreshInt();
		return (m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF;
	}

	/// @return A random uint with a maximum
	uint NextUInt(uint p_max)
	{
		if (m_ints >= 2) RefreshInt();
		return ((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF) % p_max;
	}

	/// @return A random uint between a maximum and a minimum
	uint NextUint(uint p_min, uint p_max)
	{
		return NextUInt(p_max - p_min) + p_min;
	}

	/// @return A random bool
	bool NextBool()
	{
		if (m_bools >= 64) RefreshBool();
		return (m_boolRandom & (1UL << m_bools++)) > 0;
	}

	/// @brief				 Creates a random number generator
	/// @param  pseudoRandom If it is pseudorandom or not
	Random(bool pseudoRandom = false)
	{
		if (pseudoRandom)
		{
			new (&m_generator) std::mt19937_64(0xFABCD00AABBCC32); //just a random chosen number
		}
		else
		{
			std::random_device rd;
			new (&m_generator) std::mt19937_64(rd());
		}
		
		m_byteRandom = m_dis(m_generator);
		m_shortRandom = m_dis(m_generator);
		m_intRandom = m_dis(m_generator);
		m_boolRandom = m_dis(m_generator);
		m_bytes = 0;
		m_shorts = 0;
		m_ints = 0;
		m_bools = 0;
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