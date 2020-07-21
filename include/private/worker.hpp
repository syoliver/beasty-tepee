#pragma once

#include "fields_alloc.hpp"
#include "stream_request.hpp"
#include "parser.hpp"
#include <beasty-tepee/request_body_handler.hpp>
#include <beasty-tepee/request_header_handler.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core/span.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <optional>
#include <memory>

namespace tepee::server
{
  class worker : public std::enable_shared_from_this<worker>
  {
   public:
    worker(const worker&) = delete; 
    worker& operator=(worker const&) = delete;

    worker(boost::asio::io_context&                               ioc,
           boost::asio::ip::tcp::acceptor&                        acceptor,
           std::shared_ptr<tepee::server::request_header_handler> handler);

    void start();

   private:
    void accept();
    void read_request();
    void send_response(const tepee::server::response& response);
    void read_buffer(boost::beast::span<char> buf, tepee::server::request_parser_t& parser, boost::beast::error_code& ec);

    void check_deadline(boost::beast::error_code ec = boost::beast::error_code{});

    friend class request_body_handler;
   private:
    boost::asio::io_context& ioc_;

    // The acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor& acceptor_;

    // The socket for the currently connected client.
    boost::asio::ip::tcp::socket socket_{ioc_};

    // The buffer for performing reads
    boost::beast::flat_static_buffer<8192> buffer_;

    // The allocator used for the fields in the request and reply.
    alloc_t alloc_{8192};

    // The parser for reading the requests
    std::optional<request_parser_impl> parser_;

    boost::asio::basic_waitable_timer<std::chrono::steady_clock> request_deadline_{
      ioc_,
      (std::chrono::steady_clock::time_point::max)()};

    std::shared_ptr<tepee::server::request_header_handler> handler_;

    // The string-based response message.
    std::optional<response_t> string_response_;

    // The string-based response serializer.
    std::optional<boost::beast::http::response_serializer<boost::beast::http::string_body,
                                                            boost::beast::http::basic_fields<alloc_t>>>
      string_serializer_;
  };

} // namespace tepee::server
