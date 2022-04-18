#pragma once
#include <cstdint>
#include <random>

#define BYTE uint8_t
#define USHORT uint16_t

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
	BYTE NextByte()
	{
		if (m_bytes >= 8) RefreshByte();
		return static_cast<BYTE>((m_byteRandom >> (8 * m_bytes++)) & 0xFF);
	}

	/// @return A random byte with a maximum
	BYTE NextByte(BYTE p_max)
	{
		if (m_bytes >= 8) RefreshByte();
		return static_cast<BYTE>(((m_byteRandom >> (8 * m_bytes++)) & 0xFF) % p_max);
	}

	/// @return A random byte between a maximum and a minimum
	BYTE NextByte(BYTE p_min, BYTE p_max)
	{
		return static_cast<BYTE>(NextByte(p_max - p_min) + p_min);
	}

	/// @return A random ushort
	USHORT NextUShort()
	{
		if (m_shorts >= 4) RefreshShort();
		return static_cast<USHORT>((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF);
	}

	/// @return A random ushort with a maximum
	USHORT NextUShort(USHORT p_max)
	{
		if (m_shorts >= 4) RefreshShort();
		return static_cast<USHORT>(((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF) % p_max);
	}

	/// @return A random ushort between a maximum and a minimum
	USHORT NextUShort(USHORT p_min, USHORT p_max)
	{
		return static_cast<USHORT>(NextUShort(static_cast<USHORT>(p_max - p_min)) + p_min);
	}

	/// @return A random short
	short NextShort()
	{
		if (m_shorts >= 4) RefreshShort();
		return static_cast<short>((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF);
	}

	/// @return A random short with a maximum
	short NextShort(short p_max)
	{
		if (m_shorts >= 4) RefreshShort();
		return static_cast<short>(((m_shortRandom >> (16 * m_shorts++)) & 0xFFFF) % p_max);
	}

	/// @return A random short between a maximum and a minimum
	short NextShort(short p_min, short p_max)
	{
		return static_cast<short>(NextShort(static_cast<short>(p_max - p_min)) + p_min);
	}

	/// @return A random int
	int NextInt()
	{
		if (m_ints >= 2) RefreshInt();
		return static_cast<int>((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF);
	}

	/// @return A random int with a maximum
	int NextInt(int p_max)
	{
		if (m_ints >= 2) RefreshInt();
		return static_cast<int>(((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF) % p_max);
	}

	/// @return A random int between a maximum and a minimum
	int NextInt(int p_min, int p_max)
	{
		return NextInt(p_max - p_min) + p_min;
	}

	/// @return A random uint
	unsigned int NextUInt()
	{
		if (m_ints >= 2) RefreshInt();
		return static_cast<unsigned int>((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF);
	}

	/// @return A random uint with a maximum
	unsigned int NextUInt(unsigned int p_max)
	{
		if (m_ints >= 2) RefreshInt();
		return static_cast<unsigned int>(((m_intRandom >> (32 * m_ints++)) & 0xFFFFFFFF) % p_max);
	}

	/// @return A random uint between a maximum and a minimum
	unsigned int NextUint(unsigned int p_min, unsigned int p_max)
	{
		return NextUInt(p_max - p_min) + p_min;
	}

	/// @return A random bool
	bool NextBool()
	{
		if (m_bools >= 64) RefreshBool();
		return (m_boolRandom & (1UL << m_bools++)) > 0;
	}

	/// @brief Creates a random number generator
	Random()
		: m_bytes(0),
		m_shorts(0),
		m_ints(0),
		m_bools(0)
	{
		std::random_device rd;
		new (&m_generator) std::mt19937_64(rd());

		m_byteRandom = m_dis(m_generator);
		m_shortRandom = m_dis(m_generator);
		m_intRandom = m_dis(m_generator);
		m_boolRandom = m_dis(m_generator);
	}

	/// @brief		   Creates a pseudo random number generator with a seed
	/// @param  p_seed The seed of the generator
	explicit Random(long long p_seed)
		: m_bytes(0),
		m_shorts(0),
		m_ints(0),
		m_bools(0),
	    m_generator(p_seed)
	{
		m_byteRandom = m_dis(m_generator);
		m_shortRandom = m_dis(m_generator);
		m_intRandom = m_dis(m_generator);
		m_boolRandom = m_dis(m_generator);
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