#pragma once
#include <cstdint>
#include <random>

#define BYTE   uint8_t
#define USHORT uint16_t

#define SHORT_MAXIMUM_VALUE 65535
#define SHORT_MAX_VALUE     65536

#define BOOL_BITS  1
#define BYTE_BITS  8
#define SHORT_BITS 16
#define INT_BITS   32

#define RANDOM_NUMBER_BYTES INT_BITS

#define BOOL_AMOUNT  (RANDOM_NUMBER_BYTES / BOOL_BITS)
#define BYTE_AMOUNT  (RANDOM_NUMBER_BYTES / BYTE_BITS)
#define SHORT_AMOUNT (RANDOM_NUMBER_BYTES / SHORT_BITS)
#define INT_AMOUNT   (RANDOM_NUMBER_BYTES / INT_BITS)

/// @brief Creates random numbers
class Random
{
public:
    /// @return A random float
    float NextFloat()
    {
        return static_cast<float>(NextUShort()) / SHORT_MAX_VALUE;
    }

    /// @return A random float with a maximum
    float NextFloat(float p_max)
    {
        return static_cast<float>(NextUShort()) / (SHORT_MAX_VALUE / p_max);
    }

    /// @return A random float between a max and a min
    float NextFloat(float p_min, float p_max)
    {
        return NextFloat(p_max - p_min) + p_min;
    }

    /// @return A random float with a max value (included)
    float NextFloatIncl(float p_max)
    {
        return static_cast<float>(NextUShort()) / (SHORT_MAXIMUM_VALUE / p_max);
    }

    /// @return A random float between a max (included) and a min
    float NextFloatIncl(float p_min, float p_max)
    {
        return NextFloatIncl(p_max - p_min) + p_min;
    }

    /// @return A random byte
    BYTE NextByte()
    {
        if (m_bytes >= BYTE_AMOUNT) RefreshByte();
        return static_cast<BYTE>((m_byteRandom >> (BYTE_BITS * m_bytes++)) & 0xFF);
    }

    /// @return A random byte with a maximum
    BYTE NextByte(BYTE p_max)
    {
        if (m_bytes >= BYTE_AMOUNT) RefreshByte();
        return static_cast<BYTE>(((m_byteRandom >> (BYTE_BITS * m_bytes++)) & 0xFF) % p_max);
    }

    /// @return A random byte between a maximum and a minimum
    BYTE NextByte(BYTE p_min, BYTE p_max)
    {
        return static_cast<BYTE>(NextByte(p_max - p_min) + p_min);
    }

    /// @return A random ushort
    USHORT NextUShort()
    {
        if (m_shorts >= SHORT_AMOUNT) RefreshShort();
        return static_cast<USHORT>((m_shortRandom >> (SHORT_BITS * m_shorts++)) & 0xFFFF);
    }

    /// @return A random ushort with a maximum
    USHORT NextUShort(USHORT p_max)
    {
        if (m_shorts >= SHORT_AMOUNT) RefreshShort();
        return static_cast<USHORT>(((m_shortRandom >> (SHORT_BITS * m_shorts++)) & 0xFFFF) % p_max);
    }

    /// @return A random ushort between a maximum and a minimum
    USHORT NextUShort(USHORT p_min, USHORT p_max)
    {
        return static_cast<USHORT>(NextUShort(static_cast<USHORT>(p_max - p_min)) + p_min);
    }

    /// @return A random short
    short NextShort()
    {
        if (m_shorts >= SHORT_AMOUNT) RefreshShort();
        return static_cast<short>((m_shortRandom >> (SHORT_BITS * m_shorts++)) & 0xFFFF);
    }

    /// @return A random short with a maximum
    short NextShort(short p_max)
    {
        if (m_shorts >= SHORT_AMOUNT) RefreshShort();
        return static_cast<short>(((m_shortRandom >> (SHORT_BITS * m_shorts++)) & 0xFFFF) % p_max);
    }

    /// @return A random short between a maximum and a minimum
    short NextShort(short p_min, short p_max)
    {
        return static_cast<short>(NextShort(static_cast<short>(p_max - p_min)) + p_min);
    }

    /// @return A random int
    int NextInt()
    {
        RefreshInt();
        return m_intRandom;
    }

    /// @return A random int with a maximum
    int NextInt(int p_max)
    {
        RefreshInt();
        return m_intRandom % p_max;
    }

    /// @return A random int between a maximum and a minimum
    int NextInt(int p_min, int p_max)
    {
        return NextInt(p_max - p_min) + p_min;
    }

    /// @return A random uint
    unsigned int NextUInt()
    {
        RefreshInt();
        return static_cast<unsigned int>(m_intRandom);
    }

    /// @return A random uint with a maximum
    unsigned int NextUInt(unsigned int p_max)
    {
        RefreshInt();
        return static_cast<unsigned int>(m_intRandom) % p_max;
    }

    /// @return A random uint between a maximum and a minimum
    unsigned int NextUint(unsigned int p_min, unsigned int p_max)
    {
        return NextUInt(p_max - p_min) + p_min;
    }

    /// @return A random bool
    bool NextBool()
    {
        if (m_bools >= BOOL_AMOUNT) RefreshBool();
        return (m_boolRandom & (1U << m_bools++)) > 0;
    }

    long long GetSeed()
    {
        return m_seed;
    }

    /// @brief Creates a random number generator
    Random()
        : m_bytes(0),
          m_shorts(0),
          m_ints(0),
          m_bools(0)
    {
        std::random_device rd;
        m_seed = rd();
        new (&m_generator) std::mt19937_64(m_seed);

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
          m_seed(p_seed),
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

    int m_byteRandom;
    int m_bytes;  // bytes and shorts are promoted to ints when doing ++

    int m_shortRandom;
    int m_shorts;

    int m_intRandom;
    int m_ints;

    unsigned int m_boolRandom;
    int m_bools;

    long long m_seed;

    std::uniform_int_distribution<int> m_dis;
    std::mt19937_64 m_generator;
};