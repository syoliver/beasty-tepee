#pragma once

#include <beasty-tepee/response.hpp>
#include <boost/beast/core/span.hpp>
#include <boost/beast/core/error.hpp>
#include <memory>

namespace tepee::server
{
    class worker;
    class stream_request;
    class request_parser;

    class request_body_handler
    {
     public:
      virtual void parse_request_body(std::shared_ptr<tepee::server::worker> worker,
                                      tepee::server::request_parser&         parser) = 0;

     protected:
      void send_response_to_worker(tepee::server::worker& worker, const tepee::server::response& response);

      void read_buffer_from_worker(tepee::server::worker&         worker,
                                   boost::beast::span<char>       buffer,
                                   tepee::server::request_parser& parser,
                                   boost::beast::error_code&      ec);
    };

    class stream_request_body_handler : public request_body_handler
    {
     public:
      using handle_t = std::function<tepee::server::response(const tepee::server::stream_request& request)>;

      stream_request_body_handler(const handle_t& handle) : handle_(handle) {}

      virtual void parse_request_body(std::shared_ptr<tepee::server::worker> worker,
                                      tepee::server::request_parser&         parser) override;

      handle_t handle_;
    };
  } // namespace tepee::server