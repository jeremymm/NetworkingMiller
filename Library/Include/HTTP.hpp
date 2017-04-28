//***************************************************************************//
// HTTP Class Interface
//
// Created: Mar 25, 2006
// By: Jeremy Michael Miller
//
// Copyright (c) 2006-2016 Jeremy Michael Miller. 
// Author: = "Jeremy Michael Miller"
// Copyright: = "Copyright 2005-2016, Networking Miller,  All rights reserved."
// Credits = ["Jeremy Michael Miller"]
// License: "Fair use v0.9"
// Version: "0.0.1"
// Maintainer: "Jeremy Michael Miller"
// Email: "maybe_later@mst.dnsalias.net"
// Status: "Alpha"
//***************************************************************************//

#ifndef MST_HTTP_HPP
#define MST_HTTP_HPP

//***************************************************************************//
// System Includes
#include <string>
//***************************************************************************//

#include "EndPoint.hpp"

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  enum HTTPStatusCode
  {
    // Informational 1xx
    HTTP_CONTINUE                   = 100,
    HTTP_SWITCHING_PROTOCOLS        = 101,
    // Successful 2xx
    HTTP_OK                         = 200,
    HTTP_CREATED                    = 201,
    HTTP_ACCEPTED                   = 202,
    HTTP_NON_AUTHORITATIVE_INFO     = 203,
    HTTP_NO_CONTENT                 = 204,
    HTTP_RESET_CONTENT              = 205,
    HTTP_PARTIAL_CONTENT            = 206,
    // Redirection 3xx
    HTTP_MULTIPLE_CHOICES           = 300,
    HTTP_MOVED                      = 301,
    HTTP_FOUND                      = 302,
    HTTP_SEE_OTHER                  = 303,
    HTTP_NOT_MODIFIED               = 304,
    HTTP_USE_PROXY                  = 305,
    HTTP_UNUSED                     = 306,
    HTTP_TEMP                       = 307,
    // Client Error 4xx
    HTTP_BAD_REQUEST                = 400,
    HTTP_UNAUTHORIZED               = 401,
    HTTP_PAYMENT_REQUIRED           = 402,
    HTTP_FORBIDDEN                  = 403,
    HTTP_NOT_FOUND                  = 404,
    HTTP_METHOD_NOT_ALLOWED         = 405,
    HTTP_NOT_ACCEPTABLE             = 406,
    HTTP_PROXY_AUTH_REQUIRED        = 407,
    HTTP_REQUEST_TIMEOUT            = 408,
    HTTP_CONFLICT                   = 409,
    HTTP_GONE                       = 410,
    HTTP_LENGTH_REQUIRED            = 411,
    HTTP_PRECONDITION_FAILED        = 412,
    HTTP_REQ_ENTITY_TOO_LARGE       = 413,
    HTTP_REQ_URI_TOO_LONG           = 414,
    HTTP_UNSUPPORTED_MEDIA_TYPE     = 415,
    HTTP_REQ_RANGE_NOT_STATISFIABLE = 416,
    HTTP_EXCEPTATION_FAILED         = 417,
    // Server Error 5xx
    HTTP_INTERNAL                   = 500,
    HTTP_NOT_IMPLEMENTED            = 501,
    HTTP_BAD_GATEWAY                = 502,
    HTTP_SERVICE_UNAVAILABLE        = 503,
    HTTP_GATEWAT_TIMEOUT            = 504,
    HTTP_VERSION_NOT_SUPPORTED      = 505
  }; // End of enum HTTPStatusCode
  //*************************************************************************//

  enum HTTPRequestMethod
  {
    HTTP_REQUEST_METHOD_HEAD,
    HTTP_REQUEST_METHOD_GET,
    HTTP_REQUEST_METHOD_POST,
    HTTP_REQUEST_METHOD_PUT,
    HTTP_REQUEST_METHOD_DELETE,
    HTTP_REQUEST_METHOD_CONNECT,
    HTTP_REQUEST_METHOD_PATCH,
    //------------------------
    HTTP_REQUEST_METHOD_UNKNOWN
  };

  //*************************************************************************//
  class HTTPRequest
  {
  private:
    //***********************************************************************//
    HTTPRequestMethod meMethod;
    std::string mstrRequest;
    std::string mstrPath;
    std::string mstrFile;

    EndPoint mLocalEndpoint;

    unsigned long mulRangeOffset;
    unsigned long mulRangeLength;
    //***********************************************************************//
  public:
    //***********************************************************************//
    HTTPRequest();
    ~HTTPRequest();
    void SetMethod(HTTPRequestMethod eMethod);
    void SetPath(const std::string& rstrPath);
    void SetFile(const std::string& rstrFile);
    void SetHost(const EndPoint& rLocalEndPoint);
    void SetRange(unsigned long ulOffset, unsigned long ulLength);

    void GenerateString();
    const std::string& String() const;
    //***********************************************************************//
  };
  //*************************************************************************//

  //*************************************************************************//
  class HTTPResponse
  {
  private:
    //***********************************************************************//
    size_t mulParsedIndex;
    // Response unparsed
    std::string mstrResponse;
    
    // Parsed Response fields
    std::string mstrDate;
    std::string mstrServer;
    std::string mstrLastModified;
    unsigned long mulContentLength;
    std::string mstrConnection;

    char* cpData;
    unsigned long mulDataSize;
    //***********************************************************************//
  public:
    //***********************************************************************//
    HTTPResponse();
    ~HTTPResponse();

    //------------------------ Response Fields ------------------------------//
    unsigned long AcceptRanges();
    unsigned long ContentLength() const;
    std::string Date() const;
    //-----------------------------------------------------------------------//

    char* const Data();
    unsigned long DataSize() const;
    void GenerateString();
    void ParseString(const std::string& rstrResponse);


    void String(const std::string& rstrResponse);
    const std::string& String() const;
    //***********************************************************************//
  };
  //*************************************************************************//

  //*************************************************************************//
  class HTTP
  {
  private:
  protected:
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    // Constructor
    HTTP();
    // Destructor
    virtual ~HTTP();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class HTTP
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_HTTP_HPP
