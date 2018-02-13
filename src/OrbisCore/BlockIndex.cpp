// Copyright (c) 2018 The Orbis developers
// ======================================================================
//              ===== Spell of Creation =====
// script inspired by the CNC code, on February 11, 2018
// 
// ======================================================================
/*                ,     
			     d 
                 :8;        oo 
                ,888     od88 Y 
                " b     "`88888  ,. 
                  :    88888888o. 
                   Y.   `8888888bo 
                    "bo  d888888888oooooo,. 
                      "Yo8:8888888888888Yd8. 
                        `8b;Y88888888888888' 
                        ,888d"Y888888888888.  , 
                        P88Y8b.888888888888b.         `b 
                        :;  "8888888888888888P8b       d; 
                         .   Y88888888888888bo   ",o ,d8 
                              `"8888888888888888888888",oo. 
                               :888888888888888888888P""   ` 
                               `88888888888888888888oooooo. : 
                            ;  ,888888Y888888888888888888888" 
                            `'`P""8.`8;`888888888888888888888o. 
                              `    ` :;  `888888888888888"""8P"o. 
                                 ,  '`8    Y88888888888888;  :b Yb 
                                  8.  :.    `Y8888888888888; dP  `8 
                                ,8d8    "     `888888888888d      Y; 
                                :888d8;        88888888888Pb       ; 
                                :888' `   o   d888888888888;      :' 
                               oo888bo  ,."8888888888888888;    ' ' 
                               8888888888888888888888888888; 
                       ,.`88booo`Y888888888888888888888888P' 
                           :888888888888888888888888888888' 
                   ,ooooood888888888888888888888888888888' 
                  ""888888888888888888888888888888888888; 
             ,oooooo888888888888888888888888888888888888' 
               "88888888888888888888888888888888888888P 
       ,oo bo ooo88888888888888888888888888888888888Y8 
     ."8P88d888888888888888888888888888888888888888"`"o. 
      oo888888888888888888888888888888888888888888"    8 
     d88Y8888888888888888888888888888888888888888' ooo8bYooooo. 
    ,""o888888888888888888888888888888888P":888888888888888888b 
    `   ,d88888888888888888888888888888"'  :888888888888888bod8 
      ,88888888888888888888888888888"      `d8888888888888o`88"b 
    ,88888888888888888888888888""            ,88888' 88  Y8b 
    " ,8888888888888888888""        ,;       88' ;   `8'  P 
     d8888888888888888888boo8888888P"         :.     ` 
    d888888888888888888888888888888boooo 
   :"Y888888888888P':88888"""8P"88888P' 
   ` :88888888888'   88""  ,dP' :888888. 
    ,88888888888'          '`  ,88,8b d" 
    d88888888888               dP"`888' 
    Y :888888888;                   8' 
      :8888888888.                 ` 
      :888888888888oo                        ,ooooo 
      :8888888888888o              ,o   oo d88888oooo. 
       ' :888888888888888booooood888888888888888888888bo.  -hrr- 
          Y88888888888888888888888888888888888"""888888o. 
           "`"Y8888888888888888888888888""""'     `"88888b. 
               "888"Y888888888888888"                Y888.' 
                `"'  `""' `"""""'  "          ,       8888 
                                              :.      8888 
                                           d888d8o    88;` 
                                           Y888888;  d88; 
                                         o88888888,o88P:' 
                                    "ood888888888888"' 
                                ,oo88888""888888888. 
                              ,o8P"b8YP  '`"888888;"b. 
                           ' d8"`o8",P    """""""8P 
                                    `;          d8; 
                                                 8;
// =============================================================
//              ===== it's not an illusion =====
//                  ===== it's real =====
//
// =============================================================
*/                          
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "BlockIndex.h"

#include <boost/utility/value_init.hpp>

#include "OrbisSerialization.h"
#include "Serialization/SerializationOverloads.h"

namespace Orbis {
  Crypto::Hash BlockIndex::getBlockId(uint32_t height) const {
    assert(height < m_container.size());

    return m_container[static_cast<size_t>(height)];
  }

  std::vector<Crypto::Hash> BlockIndex::getBlockIds(uint32_t startBlockIndex, uint32_t maxCount) const {
    std::vector<Crypto::Hash> result;
    if (startBlockIndex >= m_container.size()) {
      return result;
    }

    size_t count = std::min(static_cast<size_t>(maxCount), m_container.size() - static_cast<size_t>(startBlockIndex));
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      result.push_back(m_container[startBlockIndex + i]);
    }

    return result;
  }

  bool BlockIndex::findSupplement(const std::vector<Crypto::Hash>& ids, uint32_t& offset) const {
    for (const auto& id : ids) {
      if (getBlockHeight(id, offset)) {
        return true;
      }
    }

    return false;
  }

  std::vector<Crypto::Hash> BlockIndex::buildSparseChain(const Crypto::Hash& startBlockId) const {
    assert(m_index.count(startBlockId) > 0);

    uint32_t startBlockHeight;
    getBlockHeight(startBlockId, startBlockHeight);

    std::vector<Crypto::Hash> result;
    size_t sparseChainEnd = static_cast<size_t>(startBlockHeight + 1);
    for (size_t i = 1; i <= sparseChainEnd; i *= 2) {
      result.emplace_back(m_container[sparseChainEnd - i]);
    }

    if (result.back() != m_container[0]) {
      result.emplace_back(m_container[0]);
    }

    return result;
  }

  Crypto::Hash BlockIndex::getTailId() const {
    assert(!m_container.empty());
    return m_container.back();
  }

  void BlockIndex::serialize(ISerializer& s) {
    if (s.type() == ISerializer::INPUT) {
      readSequence<Crypto::Hash>(std::back_inserter(m_container), "index", s);
    } else {
      writeSequence<Crypto::Hash>(m_container.begin(), m_container.end(), "index", s);
    }
  }
}
