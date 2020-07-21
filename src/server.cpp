#include <beasty-tepee/server.hpp>
#include "server_impl.hpp"
#include <beasty-tepee/request_body_handler.hpp>


namespace tepee::server
{
  server::server(boost::asio::io_context&                               ioc,
                 std::shared_ptr<tepee::server::request_header_handler> handler,
                 std::uint64_t                                          number_of_workers)
      : impl_(std::make_shared<server_impl>(ioc, handler, number_of_workers))
  {
  }

  void server::listen(const std::string& address, std::uint16_t port)
  {
    impl_->listen(address, port);
  }
} // namespace tepee::server