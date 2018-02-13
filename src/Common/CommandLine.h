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

#include <iostream>
#include <type_traits>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

namespace command_line
{
  template<typename T, bool required = false>
  struct arg_descriptor;

  template<typename T>
  struct arg_descriptor<T, false>
  {
    typedef T value_type;

    const char* name;
    const char* description;
    T default_value;
    bool not_use_default;
  };

  template<typename T>
  struct arg_descriptor<std::vector<T>, false>
  {
    typedef std::vector<T> value_type;

    const char* name;
    const char* description;
  };

  template<typename T>
  struct arg_descriptor<T, true>
  {
    static_assert(!std::is_same<T, bool>::value, "Boolean switch can't be required");

    typedef T value_type;

    const char* name;
    const char* description;
  };

  template<typename T>
  boost::program_options::typed_value<T, char>* make_semantic(const arg_descriptor<T, true>& /*arg*/)
  {
    return boost::program_options::value<T>()->required();
  }

  template<typename T>
  boost::program_options::typed_value<T, char>* make_semantic(const arg_descriptor<T, false>& arg)
  {
    auto semantic = boost::program_options::value<T>();
    if (!arg.not_use_default)
      semantic->default_value(arg.default_value);
    return semantic;
  }

  template<typename T>
  boost::program_options::typed_value<T, char>* make_semantic(const arg_descriptor<T, false>& arg, const T& def)
  {
    auto semantic = boost::program_options::value<T>();
    if (!arg.not_use_default)
      semantic->default_value(def);
    return semantic;
  }

  template<typename T>
  boost::program_options::typed_value<std::vector<T>, char>* make_semantic(const arg_descriptor<std::vector<T>, false>& /*arg*/)
  {
    auto semantic = boost::program_options::value< std::vector<T> >();
    semantic->default_value(std::vector<T>(), "");
    return semantic;
  }

  template<typename T, bool required>
  void add_arg(boost::program_options::options_description& description, const arg_descriptor<T, required>& arg, bool unique = true)
  {
    if (unique && 0 != description.find_nothrow(arg.name, false))
    {
      std::cerr << "Argument already exists: " << arg.name << std::endl;
      return;
    }

    description.add_options()(arg.name, make_semantic(arg), arg.description);
  }

  template<typename T>
  void add_arg(boost::program_options::options_description& description, const arg_descriptor<T, false>& arg, const T& def, bool unique = true)
  {
    if (unique && 0 != description.find_nothrow(arg.name, false))
    {
      std::cerr << "Argument already exists: " << arg.name << std::endl;
      return;
    }

    description.add_options()(arg.name, make_semantic(arg, def), arg.description);
  }

  template<>
  inline void add_arg(boost::program_options::options_description& description, const arg_descriptor<bool, false>& arg, bool unique)
  {
    if (unique && 0 != description.find_nothrow(arg.name, false))
    {
      std::cerr << "Argument already exists: " << arg.name << std::endl;
      return;
    }

    description.add_options()(arg.name, boost::program_options::bool_switch(), arg.description);
  }

  template<typename charT>
  boost::program_options::basic_parsed_options<charT> parse_command_line(int argc, const charT* const argv[],
    const boost::program_options::options_description& desc, bool allow_unregistered = false)
  {
    auto parser = boost::program_options::command_line_parser(argc, argv);
    parser.options(desc);
    if (allow_unregistered)
    {
      parser.allow_unregistered();
    }
    return parser.run();
  }

  template<typename F>
  bool handle_error_helper(const boost::program_options::options_description& desc, F parser)
  {
    try
    {
      return parser();
    }
    catch (std::exception& e)
    {
      std::cerr << "Failed to parse arguments: " << e.what() << std::endl;
      std::cerr << desc << std::endl;
      return false;
    }
    catch (...)
    {
      std::cerr << "Failed to parse arguments: unknown exception" << std::endl;
      std::cerr << desc << std::endl;
      return false;
    }
  }

  template<typename T, bool required>
  bool has_arg(const boost::program_options::variables_map& vm, const arg_descriptor<T, required>& arg)
  {
    auto value = vm[arg.name];
    return !value.empty();
  }


  template<typename T, bool required>
  T get_arg(const boost::program_options::variables_map& vm, const arg_descriptor<T, required>& arg)
  {
    return vm[arg.name].template as<T>();
  }
 
  template<>
  inline bool has_arg<bool, false>(const boost::program_options::variables_map& vm, const arg_descriptor<bool, false>& arg)
  {
    return get_arg<bool, false>(vm, arg);
  }


  extern const arg_descriptor<bool> arg_help;
  extern const arg_descriptor<bool> arg_version;
  extern const arg_descriptor<std::string> arg_data_dir;
}
