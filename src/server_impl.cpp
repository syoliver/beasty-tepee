#include "server_impl.hpp"
#include <beasty-tepee/request_body_handler.hpp>
#include <boost/asio/ip/address.hpp>
#include <deque>
#include <functional>
#include <iostream>
#include <optional>
#include <tuple>


namespace tepee::server
{
  server_impl::server_impl(boost::asio::io_context&                               ioc,
                           std::shared_ptr<tepee::server::request_header_handler> handler,
                           std::uint64_t                                          number_of_workers)
      : ioc_(ioc)
      , handler_(handler)
      , number_of_workers_(number_of_workers)
  {
  }

  void server_impl::listen(const std::string& address, std::uint16_t port)
  {
    acceptor_.emplace(ioc_, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port));

    for(std::uint64_t i = 0; i < number_of_workers_; ++i)
    {
      auto worker = std::make_shared<tepee::server::worker>(ioc_, acceptor_.value(), handler_);
      workers_.push_back(worker);
      worker->start();
    }
  }
} // namespace tepee::server