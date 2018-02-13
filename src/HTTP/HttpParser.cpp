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

#include "HttpParser.h"

#include <algorithm>

#include "HttpParserErrorCodes.h"

namespace {

void throwIfNotGood(std::istream& stream) {
  if (!stream.good()) {
    if (stream.eof()) {
      throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::END_OF_STREAM));
    } else {
      throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::STREAM_NOT_GOOD));
    }
  }
}

}

namespace Orbis {

HttpResponse::HTTP_STATUS HttpParser::parseResponseStatusFromString(const std::string& status) {
  if (status == "200 OK" || status == "200 Ok") return Orbis::HttpResponse::STATUS_200;
  else if (status == "404 Not Found") return Orbis::HttpResponse::STATUS_404;
  else if (status == "500 Internal Server Error") return Orbis::HttpResponse::STATUS_500;
  else throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::UNEXPECTED_SYMBOL),
      "Unknown HTTP status code is given");

  return Orbis::HttpResponse::STATUS_200; //unaccessible
}


void HttpParser::receiveRequest(std::istream& stream, HttpRequest& request) {
  readWord(stream, request.method);
  readWord(stream, request.url);

  std::string httpVersion;
  readWord(stream, httpVersion);

  readHeaders(stream, request.headers);

  std::string body;
  size_t bodyLen = getBodyLen(request.headers);
  if (bodyLen) {
    readBody(stream, request.body, bodyLen);
  }
}


void HttpParser::receiveResponse(std::istream& stream, HttpResponse& response) {
  std::string httpVersion;
  readWord(stream, httpVersion);
  
  std::string status;
  char c;
  
  stream.get(c);
  while (stream.good() && c != '\r') { //Till the end
    status += c;
    stream.get(c);
  }

  throwIfNotGood(stream);

  if (c == '\r') {
    stream.get(c);
    if (c != '\n') {
      throw std::runtime_error("Parser error: '\\n' symbol is expected");
    }
  }

  response.setStatus(parseResponseStatusFromString(status));
  
  std::string name;
  std::string value;

  while (readHeader(stream, name, value)) {
    response.addHeader(name, value);
    name.clear();
    value.clear();
  }

  response.addHeader(name, value);
  auto headers = response.getHeaders();
  size_t length = 0;
  auto it = headers.find("content-length");
  if (it != headers.end()) {
    length = std::stoul(it->second);
  }
  
  std::string body;
  if (length) {
    readBody(stream, body, length);
  }

  response.setBody(body);
}


void HttpParser::readWord(std::istream& stream, std::string& word) {
  char c;

  stream.get(c);
  while (stream.good() && c != ' ' && c != '\r') {
    word += c;
    stream.get(c);
  }

  throwIfNotGood(stream);

  if (c == '\r') {
    stream.get(c);
    if (c != '\n') {
      throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::UNEXPECTED_SYMBOL));
    }
  }
}

void HttpParser::readHeaders(std::istream& stream, HttpRequest::Headers& headers) {
  std::string name;
  std::string value;

  while (readHeader(stream, name, value)) {
    headers[name] = value; //use insert
    name.clear();
    value.clear();
  }

  headers[name] = value; //use insert
}

bool HttpParser::readHeader(std::istream& stream, std::string& name, std::string& value) {
  char c;
  bool isName = true;

  stream.get(c);
  while (stream.good() && c != '\r') {
    if (c == ':') {
      if (stream.peek() == ' ') {
        stream.get(c);
      }

      if (name.empty()) {
        throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::EMPTY_HEADER));
      }

      if (isName) {
        isName = false;
        stream.get(c);
        continue;
      }
    }

    if (isName) {
      name += c;
      stream.get(c);
    } else {
      value += c;
      stream.get(c);
    }
  }

  throwIfNotGood(stream);

  stream.get(c);
  if (c != '\n') {
    throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::UNEXPECTED_SYMBOL));
  }

  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  c = stream.peek();
  if (c == '\r') {
    stream.get(c).get(c);
    if (c != '\n') {
      throw std::system_error(make_error_code(Orbis::error::HttpParserErrorCodes::UNEXPECTED_SYMBOL));
    }

    return false; //no more headers
  }

  return true;
}

size_t HttpParser::getBodyLen(const HttpRequest::Headers& headers) {
  auto it = headers.find("content-length");
  if (it != headers.end()) {
    size_t bytes = std::stoul(it->second);
    return bytes;
  }

  return 0;
}

void HttpParser::readBody(std::istream& stream, std::string& body, const size_t bodyLen) {
  size_t read = 0;

  while (stream.good() && read < bodyLen) {
    body += stream.get();
    ++read;
  }

  throwIfNotGood(stream);
}

}
