# Orbis
This is the reference code for [Orbis](https://orbistransfer.com) orbiscurrency protocol. //to decide website, etc...

* Launch your own Orbis currency: [Orbis Starter](https://orbistransfer.com)
* Orbis reference implementation: [Orbis Coin](https://orbistransfer.com)
* Discussion board and support: [Orbis Forum](https://forum.orbistransfer.com)


# The Emission Logic of Orbis

**1. Total money supply** (src/OrbisConfig.h)

Total amount of coins to be emitted. Most of CryptoNote based coins use `(uint64_t)(-1)` (equals to 18446744073709551616). We define an explicitly numer for Orbis (`UINT64_C(11110000000000000000)`).

Example:
```
const uint64_t MONEY_SUPPLY = (uint64_t)(-1);
```

**2. Emission curve** (src/OrbisConfig.h)

`EMISSION_SPEED_FACTOR` constant defines emission curve slope. This parameter is required to calulate block reward. 

Used in Orbis:
```
const unsigned EMISSION_SPEED_FACTOR = 21;
```

**3. Difficulty target** (src/OrbisConfig.h)

Difficulty target is an ideal time period between blocks. In case an average time between blocks becomes less than difficulty target, the difficulty increases. Difficulty target is measured in seconds.

Difficulty target directly influences several aspects of coin's behavior:

- transaction confirmation speed: the longer the time between the blocks is, the slower transaction confirmation is
- emission speed: the longer the time between the blocks is the slower the emission process is
- orphan rate: chains with very fast blocks have greater orphan rate

For most coins difficulty target is 60 or 120 seconds.

Used in Orbis:
```
const uint64_t DIFFICULTY_TARGET = 75;
```

**4. Block reward formula**

In case you are not satisfied with Orbis default implementation of block reward logic you can also change it. The implementation is in `src/OrbisCore/Currency.cpp`:
```
bool Currency::getBlockReward(size_t medianSize, size_t currentBlockSize, uint64_t alreadyGeneratedCoins, uint64_t fee, uint64_t& reward, int64_t& emissionChange) const
```

This function has two parts:

- basic block reward calculation: `uint64_t baseReward = (m_moneySupply - alreadyGeneratedCoins) >> m_emissionSpeedFactor;`
- big block penalty calculation: this is the way Orbis protects the block chain from transaction flooding attacks and preserves opportunities for organic network growth at the same time.

Only the first part of this function is directly related to the emission logic. You can change it the way you want. See MonetaVerde and DuckNote as the examples where this function is modified.

# The Emission Logic of Orbis compared with Bitcoin

<p><a href="http://jsfiddle.net/bd78d290/15/" target="_blank" title="Orbis emission">Orbis emission Logic</a></p>

