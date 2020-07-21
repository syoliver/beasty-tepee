#include "worker.hpp"
#include <beasty-tepee/request_header_handler.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/coroutine2/coroutine.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/core/span.hpp>

namespace tepee::server
{
  worker::worker(boost::asio::io_context&                               ioc,
                 boost::asio::ip::tcp::acceptor&                        acceptor,
                 std::shared_ptr<tepee::server::request_header_handler> handler)
      : acceptor_(acceptor)
      , handler_(handler)
      , ioc_(ioc)
  {
  }

  void worker::start()
  {
    accept();
    // check_deadline();
  }

  void worker::accept()
  {
    // Clean up any previous connection.
    boost::beast::error_code ec;
    socket_.close(ec);
    buffer_.consume(buffer_.size());

    auto self = shared_from_this();
    acceptor_.async_accept(socket_, [this, self](boost::beast::error_code ec) {
      if(ec)
      {
        accept();
      }
      else
      {
        // Request must be fully processed within 60 seconds.
        request_deadline_.expires_after(std::chrono::seconds(60));

        read_request();
      }
    });
  }

  void worker::read_request()
  {
    // On each read the parser needs to be destroyed and
    // recreated. We store it in a boost::optional to
    // achieve that.
    //
    // Arguments passed to the parser constructor are
    // forwarded to the message object. A single argument
    // is forwarded to the body constructor.
    //
    // We construct the dynamic body with a 1MB limit
    // to prevent vulnerability to buffer attacks.
    //
    parser_.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(alloc_));

    auto& parser = *parser_;

    boost::beast::error_code ec;
    boost::beast::http::read_header(socket_, buffer_, parser, ec);

    if(ec)
      return;

    tepee::server::header header_request(parser.get().method(), parser.get().target());
    auto&                 request_aggregator = handler_->parse_header(header_request);
    request_aggregator.parse_request_body(shared_from_this(), parser);
  }

  void worker::send_response(const tepee::server::response& response)
  {
    string_response_.emplace(std::piecewise_construct, std::make_tuple(), std::make_tuple(alloc_));

    string_response_->result(response.status());
    string_response_->keep_alive(false);
    string_response_->set(boost::beast::http::field::server, "Beast");
    string_response_->set(boost::beast::http::field::content_type, "text/plain");
    // string_response_->body() = error;
    string_response_->prepare_payload();

    string_serializer_.emplace(*string_response_);

    auto self = shared_from_this();
    boost::beast::http::async_write(socket_, string_serializer_.value(), [this, self](boost::beast::error_code ec, std::size_t) {
      socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
      string_serializer_.reset();
      string_response_.reset();
      accept();
    });
  }
  void worker::check_deadline(boost::beast::error_code ec)
  {
    if(!ec)
    {
      // The deadline may have moved, so check it has really passed.
      if(request_deadline_.expiry() <= std::chrono::steady_clock::now())
      {
        // Close socket to cancel any outstanding operation.
        socket_.close();

        // Sleep indefinitely until we're given a new deadline.
        request_deadline_.expires_at(std::chrono::steady_clock::time_point::max());
      }
      auto self = shared_from_this();
      request_deadline_.async_wait([this, self](boost::beast::error_code ec) { check_deadline(ec); });
    }
  }

  void worker::read_buffer(boost::beast::span<char> buf, tepee::server::request_parser_t& parser, boost::beast::error_code& ec)
  {
    parser.get().body().data = buf.data();
    parser.get().body().size = buf.size();
    boost::beast::http::read(socket_, buffer_, parser, ec);
  }
} // namespace tepee::server
