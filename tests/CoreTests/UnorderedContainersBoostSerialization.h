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
#pragma once

#include <boost/serialization/split_free.hpp>
#include <unordered_map>
#include <unordered_set>

#include "google/sparse_hash_set"
#include "google/sparse_hash_map"

namespace boost
{
  namespace serialization
  {
    template <class Archive, class h_key, class hval, class hfunc>
    inline void save(Archive &a, const std::unordered_map<h_key, hval, hfunc> &x, const boost::serialization::version_type ver)
    {
      size_t s = x.size();
      a << s;
      BOOST_FOREACH(auto& v, x)
      {
        a << v.first;
        a << v.second;
      }
    }

    template <class Archive, class h_key, class hval, class hfunc>
    inline void load(Archive &a, std::unordered_map<h_key, hval, hfunc> &x, const boost::serialization::version_type ver)
    {
      x.clear();
      size_t s = 0;
      a >> s;
      x.reserve(s);
      for(size_t i = 0; i != s; i++)
      {
        h_key k;
        hval v;
        a >> k;
        a >> v;
        x.insert(std::pair<h_key, hval>(k, v));
      }
    }


    template <class Archive, class h_key, class hval>
    inline void save(Archive &a, const std::unordered_multimap<h_key, hval> &x, const boost::serialization::version_type ver)
    {
      size_t s = x.size();
      a << s;
      BOOST_FOREACH(auto& v, x)
      {
        a << v.first;
        a << v.second;
      }
    }

    template <class Archive, class h_key, class hval>
    inline void load(Archive &a, std::unordered_multimap<h_key, hval> &x, const boost::serialization::version_type ver)
    {
      x.clear();
      size_t s = 0;
      a >> s;
      x.reserve(s);
      for (size_t i = 0; i != s; i++)
      {
        h_key k;
        hval v;
        a >> k;
        a >> v;
        x.emplace(k, v);
      }
    }


    template <class Archive, class hval>
    inline void save(Archive &a, const std::unordered_set<hval> &x, const boost::serialization::version_type ver)
    {
      size_t s = x.size();
      a << s;
      BOOST_FOREACH(auto& v, x)
      {
        a << v;
      }
    }

    template <class Archive, class hval>
    inline void load(Archive &a, std::unordered_set<hval> &x, const boost::serialization::version_type ver)
    {
      x.clear();
      size_t s = 0;
      a >> s;
      x.reserve(s);
      for (size_t i = 0; i != s; i++)
      {
        hval v;
        a >> v;
        x.insert(v);
      }
    }

    template <class Archive, class hval>
    inline void save(Archive &a, const ::google::sparse_hash_set<hval> &x, const boost::serialization::version_type ver)
    {
      size_t s = x.size();
      a << s;
      BOOST_FOREACH(auto& v, x)
      {
        a << v;
      }
    }

    template <class Archive, class hval>
    inline void load(Archive &a, ::google::sparse_hash_set<hval> &x, const boost::serialization::version_type ver)
    {
      x.clear();
      size_t s = 0;
      a >> s;
      x.resize(s);
      for(size_t i = 0; i != s; i++)
      {
        hval v;
        a >> v;
        x.insert(v);
      }
    }

    template <class Archive, class hval>
    inline void serialize(Archive &a, ::google::sparse_hash_set<hval> &x, const boost::serialization::version_type ver)
    {
      split_free(a, x, ver);
    }

    template <class Archive, class h_key, class hval>
    inline void save(Archive &a, const ::google::sparse_hash_map<h_key, hval> &x, const boost::serialization::version_type ver)
    {
      size_t s = x.size();
      a << s;
      BOOST_FOREACH(auto& v, x)
      {
        a << v.first;
        a << v.second;
      }
    }

    template <class Archive, class h_key, class hval>
    inline void load(Archive &a, ::google::sparse_hash_map<h_key, hval> &x, const boost::serialization::version_type ver)
    {
      x.clear();
      size_t s = 0;
      a >> s;
      x.resize(s);
      for(size_t i = 0; i != s; i++)
      {
        h_key k;
        hval v;
        a >> k;
        a >> v;
        x.insert(std::pair<h_key, hval>(k, v));
      }
    }

    template <class Archive, class h_key, class hval>
    inline void serialize(Archive &a, ::google::sparse_hash_map<h_key, hval> &x, const boost::serialization::version_type ver)
    {
      split_free(a, x, ver);
    }

    template <class Archive, class h_key, class hval, class hfunc>
    inline void serialize(Archive &a, std::unordered_map<h_key, hval, hfunc> &x, const boost::serialization::version_type ver)
    {
      split_free(a, x, ver);
    }

    template <class Archive, class h_key, class hval>
    inline void serialize(Archive &a, std::unordered_multimap<h_key, hval> &x, const boost::serialization::version_type ver)
    {
      split_free(a, x, ver);
    }

    template <class Archive, class hval>
    inline void serialize(Archive &a, std::unordered_set<hval> &x, const boost::serialization::version_type ver)
    {
      split_free(a, x, ver);
    }
  }
}
