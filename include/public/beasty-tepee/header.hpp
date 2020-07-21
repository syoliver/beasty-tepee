#pragma once

#include <boost/beast/http/verb.hpp>
#include <string_view>

namespace tepee::server {
  class header
  {
   public:
    header(boost::beast::http::verb method, std::string_view target)
        : method_(method)
        , target_(target)
  
    {
    }

    boost::beast::http::verb method() const { return method_; }
    std::string_view         target() const { return target_; }

   private:
    boost::beast::http::verb method_;
    std::string_view         target_;
  };
}