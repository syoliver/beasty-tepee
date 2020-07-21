#pragma once

#include <boost/beast/http/status.hpp>

namespace tepee::server
{

  class response
  {
   public:
    response(boost::beast::http::status status) : status_(status) {}
    response(const response&) = default;
    response(response&&) = default;

    boost::beast::http::status status() const { return status_; }
    
  private:
    boost::beast::http::status status_;
  };
}