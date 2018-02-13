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
#include "LoggerManager.h"
#include <thread>
#include "ConsoleLogger.h"
#include "FileLogger.h"

namespace Logging {

using Common::JsonValue;

LoggerManager::LoggerManager() {
}

void LoggerManager::operator()(const std::string& category, Level level, boost::posix_time::ptime time, const std::string& body) {
  std::unique_lock<std::mutex> lock(reconfigureLock);
  LoggerGroup::operator()(category, level, time, body);
}

void LoggerManager::configure(const JsonValue& val) {
  std::unique_lock<std::mutex> lock(reconfigureLock);
  loggers.clear();
  LoggerGroup::loggers.clear();
  Level globalLevel;
  if (val.contains("globalLevel")) {
    auto levelVal = val("globalLevel");
    if (levelVal.isInteger()) {
      globalLevel = static_cast<Level>(levelVal.getInteger());
    } else {
      throw std::runtime_error("parameter globalLevel has wrong type");
    }
  } else {
    globalLevel = TRACE;
  }
  std::vector<std::string> globalDisabledCategories;

  if (val.contains("globalDisabledCategories")) {
    auto globalDisabledCategoriesList = val("globalDisabledCategories");
    if (globalDisabledCategoriesList.isArray()) {
      size_t countOfCategories = globalDisabledCategoriesList.size();
      for (size_t i = 0; i < countOfCategories; ++i) {
        auto categoryVal = globalDisabledCategoriesList[i];
        if (categoryVal.isString()) {
          globalDisabledCategories.push_back(categoryVal.getString());
        }
      }
    } else {
      throw std::runtime_error("parameter globalDisabledCategories has wrong type");
    }
  }

  if (val.contains("loggers")) {
    auto loggersList = val("loggers");
    if (loggersList.isArray()) {
      size_t countOfLoggers = loggersList.size();
      for (size_t i = 0; i < countOfLoggers; ++i) {
        auto loggerConfiguration = loggersList[i];
        if (!loggerConfiguration.isObject()) {
          throw std::runtime_error("loggers element must be objects");
        }

        Level level = INFO;
        if (loggerConfiguration.contains("level")) {
          level = static_cast<Level>(loggerConfiguration("level").getInteger());
        }

        std::string type = loggerConfiguration("type").getString();
        std::unique_ptr<Logging::CommonLogger> logger;

        if (type == "console") {
          logger.reset(new ConsoleLogger(level));
        } else if (type == "file") {
          std::string filename = loggerConfiguration("filename").getString();
          auto fileLogger = new FileLogger(level);
          fileLogger->init(filename);
          logger.reset(fileLogger);
        } else {
          throw std::runtime_error("Unknown logger type: " + type);
        }

        if (loggerConfiguration.contains("pattern")) {
          logger->setPattern(loggerConfiguration("pattern").getString());
        }

        std::vector<std::string> disabledCategories;
        if (loggerConfiguration.contains("disabledCategories")) {
          auto disabledCategoriesVal = loggerConfiguration("disabledCategories");
          size_t countOfCategories = disabledCategoriesVal.size();
          for (size_t i = 0; i < countOfCategories; ++i) {
            auto categoryVal = disabledCategoriesVal[i];
            if (categoryVal.isString()) {
              logger->disableCategory(categoryVal.getString());
            }
          }
        }

        loggers.emplace_back(std::move(logger));
        addLogger(*loggers.back());
      }
    } else {
      throw std::runtime_error("loggers parameter has wrong type");
    }
  } else {
    throw std::runtime_error("loggers parameter missing");
  }
  setMaxLevel(globalLevel);
  for (const auto& category : globalDisabledCategories) {
    disableCategory(category);
  }
}

}
