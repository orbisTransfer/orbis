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

#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <functional>

#include "CoreRpcServerCommandsDefinitions.h"
#include <Common/JsonValue.h>
#include "Serialization/ISerializer.h"
#include "Serialization/SerializationTools.h"

namespace Orbis {

class HttpClient;
  
namespace JsonRpc {

const int errParseError = -32700;
const int errInvalidRequest = -32600;
const int errMethodNotFound = -32601;
const int errInvalidParams = -32602;
const int errInternalError = -32603;

class JsonRpcError: public std::exception {
public:
  JsonRpcError();
  JsonRpcError(int c);
  JsonRpcError(int c, const std::string& msg);

#ifdef _MSC_VER
  virtual const char* what() const override {
#else
  virtual const char* what() const noexcept override {
#endif
    return message.c_str();
  }

  void serialize(ISerializer& s) {
    s(code, "code");
    s(message, "message");
  }

  int code;
  std::string message;
};

typedef boost::optional<Common::JsonValue> OptionalId;

class JsonRpcRequest {
public:
  
  JsonRpcRequest() : psReq(Common::JsonValue::OBJECT) {}

  bool parseRequest(const std::string& requestBody) {
    try {
      psReq = Common::JsonValue::fromString(requestBody);
    } catch (std::exception&) {
      throw JsonRpcError(errParseError);
    }

    if (!psReq.contains("method")) {
      throw JsonRpcError(errInvalidRequest);
    }

    method = psReq("method").getString();

    if (psReq.contains("id")) {
      id = psReq("id");
    }

    return true;
  }

  template <typename T>
  bool loadParams(T& v) const {
    loadFromJsonValue(v, psReq.contains("params") ? 
      psReq("params") : Common::JsonValue(Common::JsonValue::NIL));
    return true;
  }

  template <typename T>
  bool setParams(const T& v) {
    psReq.set("params", storeToJsonValue(v));
    return true;
  }

  const std::string& getMethod() const {
    return method;
  }

  void setMethod(const std::string& m) {
    method = m;
  }

  const OptionalId& getId() const {
    return id;
  }

  std::string getBody() {
    psReq.set("jsonrpc", std::string("2.0"));
    psReq.set("method", method);
    return psReq.toString();
  }

private:

  Common::JsonValue psReq;
  OptionalId id;
  std::string method;
};


class JsonRpcResponse {
public:

  JsonRpcResponse() : psResp(Common::JsonValue::OBJECT) {}

  void parse(const std::string& responseBody) {
    try {
      psResp = Common::JsonValue::fromString(responseBody);
    } catch (std::exception&) {
      throw JsonRpcError(errParseError);
    }
  }

  void setId(const OptionalId& id) {
    if (id.is_initialized()) {
      psResp.insert("id", id.get());
    }
  }

  void setError(const JsonRpcError& err) {
    psResp.set("error", storeToJsonValue(err));
  }

  bool getError(JsonRpcError& err) const {
    if (!psResp.contains("error")) {
      return false;
    }

    loadFromJsonValue(err, psResp("error"));
    return true;
  }

  std::string getBody() {
    psResp.set("jsonrpc", std::string("2.0"));
    return psResp.toString();
  }

  template <typename T>
  bool setResult(const T& v) {
    psResp.set("result", storeToJsonValue(v));
    return true;
  }

  template <typename T>
  bool getResult(T& v) const {
    if (!psResp.contains("result")) {
      return false;
    }

    loadFromJsonValue(v, psResp("result"));
    return true;
  }

private:
  Common::JsonValue psResp;
};


void invokeJsonRpcCommand(HttpClient& httpClient, JsonRpcRequest& req, JsonRpcResponse& res);

template <typename Request, typename Response>
void invokeJsonRpcCommand(HttpClient& httpClient, const std::string& method, const Request& req, Response& res) {
  JsonRpcRequest jsReq;
  JsonRpcResponse jsRes;

  jsReq.setMethod(method);
  jsReq.setParams(req);

  invokeJsonRpcCommand(httpClient, jsReq, jsRes);

  jsRes.getResult(res);
}

template <typename Request, typename Response, typename Handler>
bool invokeMethod(const JsonRpcRequest& jsReq, JsonRpcResponse& jsRes, Handler handler) {
  Request req;
  Response res;

  if (!std::is_same<Request, Orbis::EMPTY_STRUCT>::value && !jsReq.loadParams(req)) {
    throw JsonRpcError(JsonRpc::errInvalidParams);
  }

  bool result = handler(req, res);

  if (result) {
    if (!jsRes.setResult(res)) {
      throw JsonRpcError(JsonRpc::errInternalError);
    }
  }
  return result;
}

typedef std::function<bool(void*, const JsonRpcRequest& req, JsonRpcResponse& res)> JsonMemberMethod;

template <typename Class, typename Params, typename Result>
JsonMemberMethod makeMemberMethod(bool (Class::*handler)(const Params&, Result&)) {
  return [handler](void* obj, const JsonRpcRequest& req, JsonRpcResponse& res) {
    return JsonRpc::invokeMethod<Params, Result>(
      req, res, std::bind(handler, static_cast<Class*>(obj), std::placeholders::_1, std::placeholders::_2));
  };
}


}


}
