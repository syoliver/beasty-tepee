#pragma once

#include <beasty-tepee/request_header_handler.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>

namespace tepee::server
{
  class thread_pool
  {
   public:
    thread_pool(boost::asio::io_context& ioc, std::size_t pool_size = std::thread::hardware_concurrency())
        : ioc_(ioc)
        , work_(ioc_.get_executor())
    {
      for(std::size_t i = 0; i < pool_size; ++i)
      {
        threads_.create_thread([this]() { ioc_.run(); });
      }
    }

    std::size_t number_of_threads() const { return threads_.size(); }

    ~thread_pool() { threads_.join_all(); }

   private:
    using work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    boost::asio::io_context& ioc_;
    work_guard_t             work_;
    boost::thread_group      threads_;
  };

  class server_impl;

  class server
  {
   public:
    server(boost::asio::io_context&                               ioc,
           std::shared_ptr<tepee::server::request_header_handler> handler,
           std::uint64_t                                          number_of_corenumber_of_workers);

    void listen(const std::string& address, std::uint16_t port);

   private:
    std::shared_ptr<server_impl> impl_;
  };
} // namespace tepee::server