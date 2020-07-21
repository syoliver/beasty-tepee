#pragma once

#include <beasty-tepee/response.hpp>
#include <beasty-tepee/header.hpp>

#include <beasty-tepee/request_body_handler.hpp>

#include <boost/beast/core/error.hpp>
#include <boost/beast/core/span.hpp>
#include <memory>
#include <functional>


namespace tepee::server
{
  class request_body_handler;

  class request_header_handler
  {
 public:
    virtual tepee::server::request_body_handler& parse_header(const tepee::server::header& header_request) = 0;
  };

class test_header_handler : public tepee::server::request_header_handler
{
 public:
  template <typename F>
  test_header_handler(F&& stream_handler) : stream_body_parser_(stream_handler)
  {
  }

  virtual tepee::server::request_body_handler& parse_header(const tepee::server::header& header_request) override
  {
    if(header_request.method() == boost::beast::http::verb::post)
    {
      return stream_body_parser_;
    }
    else
    {
    }
  }

  stream_request_body_handler stream_body_parser_;
};

} // tepee::server