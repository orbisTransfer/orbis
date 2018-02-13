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
#include "MiningConfig.h"

#include <iostream>
#include <thread>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "OrbisConfig.h"
#include "Logging/ILogger.h"

namespace po = boost::program_options;

namespace Orbis {

namespace {

const size_t DEFAULT_SCANT_PERIOD = 30;
const char* DEFAULT_DAEMON_HOST = "127.0.0.1";
const size_t CONCURRENCY_LEVEL = std::thread::hardware_concurrency();

po::options_description cmdOptions;

void parseDaemonAddress(const std::string& daemonAddress, std::string& daemonHost, uint16_t& daemonPort) {
  std::vector<std::string> splittedAddress;
  boost::algorithm::split(splittedAddress, daemonAddress, boost::algorithm::is_any_of(":"));

  if (splittedAddress.size() != 2) {
    throw std::runtime_error("Wrong daemon address format");
  }

  if (splittedAddress[0].empty() || splittedAddress[1].empty()) {
    throw std::runtime_error("Wrong daemon address format");
  }

  daemonHost = splittedAddress[0];

  try {
    daemonPort = boost::lexical_cast<uint16_t>(splittedAddress[1]);
  } catch (std::exception&) {
    throw std::runtime_error("Wrong daemon address format");
  }
}

}

MiningConfig::MiningConfig(): help(false) {
  cmdOptions.add_options()
      ("help,h", "produce this help message and exit")
      ("address", po::value<std::string>(), "Valid orbis miner's address")
      ("daemon-host", po::value<std::string>()->default_value(DEFAULT_DAEMON_HOST), "Daemon host")
      ("daemon-rpc-port", po::value<uint16_t>()->default_value(static_cast<uint16_t>(RPC_DEFAULT_PORT)), "Daemon's RPC port")
      ("daemon-address", po::value<std::string>(), "Daemon host:port. If you use this option you must not use --daemon-host and --daemon-port options")
      ("threads", po::value<size_t>()->default_value(CONCURRENCY_LEVEL), "Mining threads count. Must not be greater than you concurrency level. Default value is your hardware concurrency level")
      ("scan-time", po::value<size_t>()->default_value(DEFAULT_SCANT_PERIOD), "Blockchain polling interval (seconds). How often miner will check blockchain for updates")
      ("log-level", po::value<int>()->default_value(1), "Log level. Must be 0..5")
      ("limit", po::value<size_t>()->default_value(0), "Mine exact quantity of blocks. 0 means no limit")
      ("first-block-timestamp", po::value<uint64_t>()->default_value(0), "Set timestamp to the first mined block. 0 means leave timestamp unchanged")
      ("block-timestamp-interval", po::value<int64_t>()->default_value(0), "Timestamp step for each subsequent block. May be set only if --first-block-timestamp has been set."
                                                         " If not set blocks' timestamps remain unchanged");
}

void MiningConfig::parse(int argc, char** argv) {
  po::variables_map options;
  po::store(po::parse_command_line(argc, argv, cmdOptions), options);
  po::notify(options);

  if (options.count("help") != 0) {
    help = true;
    return;
  }

  if (options.count("address") == 0) {
    throw std::runtime_error("Specify --address option");
  }

  miningAddress = options["address"].as<std::string>();

  if (!options["daemon-address"].empty()) {
    if (!options["daemon-host"].defaulted() || !options["daemon-rpc-port"].defaulted()) {
      throw std::runtime_error("Either --daemon-host or --daemon-rpc-port is already specified. You must not specify --daemon-address");
    }

    parseDaemonAddress(options["daemon-address"].as<std::string>(), daemonHost, daemonPort);
  } else {
    daemonHost = options["daemon-host"].as<std::string>();
    daemonPort = options["daemon-rpc-port"].as<uint16_t>();
  }

  threadCount = options["threads"].as<size_t>();
  if (threadCount == 0 || threadCount > CONCURRENCY_LEVEL) {
    throw std::runtime_error("--threads option must be 1.." + std::to_string(CONCURRENCY_LEVEL));
  }

  scanPeriod = options["scan-time"].as<size_t>();
  if (scanPeriod == 0) {
    throw std::runtime_error("--scan-time must not be zero");
  }

  logLevel = static_cast<uint8_t>(options["log-level"].as<int>());
  if (logLevel > static_cast<uint8_t>(Logging::TRACE)) {
    throw std::runtime_error("--log-level value is too big");
  }

  blocksLimit = options["limit"].as<size_t>();

  if (!options["block-timestamp-interval"].defaulted() && options["first-block-timestamp"].defaulted()) {
    throw std::runtime_error("If you specify --block-timestamp-interval you must specify --first-block-timestamp either");
  }

  firstBlockTimestamp = options["first-block-timestamp"].as<uint64_t>();
  blockTimestampInterval = options["block-timestamp-interval"].as<int64_t>();
}

void MiningConfig::printHelp() {
  std::cout << cmdOptions << std::endl;
}

}
