#include <beasty-tepee/server.hpp>

int main()
{
  boost::asio::io_context    ioc;
  tepee::server::thread_pool pool(ioc);
  auto                       handler = std::make_shared<tepee::server::test_header_handler>(
    [](const tepee::server::stream_request& request) -> tepee::server::response {
      std::cout << "called..." << std::endl;
      std::string body = "called...";
      return tepee::server::response(boost::beast::http::status::ok, body);
  });
  tepee::server::server      server(ioc, handler, 2 * pool.number_of_threads());
  server.listen("0.0.0.0", 80);
  ioc.run();
}