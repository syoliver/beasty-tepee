#pragma once

#include "worker.hpp"
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

namespace tepee::server
{
  class server_impl
  {
   public:
    server_impl(boost::asio::io_context&                          ioc,
           std::shared_ptr<tepee::server::request_header_handler> handler,
           std::uint64_t                                          number_of_corenumber_of_workers);

    void listen(const std::string& address, std::uint16_t port);

   private:
    std::size_t                                            number_of_workers_;
    boost::asio::io_context&                               ioc_;
    std::shared_ptr<tepee::server::request_header_handler> handler_;
    std::optional<boost::asio::ip::tcp::acceptor>          acceptor_;
    std::deque<std::shared_ptr<worker>>                    workers_;
  };
} // namespace tepee::server