#pragma once

#include <boost/beast/http/status.hpp>
#include <string>

namespace tepee::server
{

  class response
  {
  public:
    response(boost::beast::http::status status, std::string body)
      : status_(status)
      , body_(std::move(body))
    {}

    response(const response&) = default;
    response(response&&) = default;


    
    boost::beast::http::status status() const { return status_; }
    void body(std::string data) { body_ = std::move(data); }
    const std::string& body() const { return body_; }


  private:
    boost::beast::http::status status_;
    std::string body_;
  };
}