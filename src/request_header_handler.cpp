
#include <boost/coroutine2/coroutine.hpp>
#include <boost/beast/core/span.hpp>
#include <beasty-tepee/request_header_handler.hpp>
#include "worker.hpp"

namespace tepee::server
{
  void request_body_handler::send_response_to_worker(tepee::server::worker&         worker,
                                                     const tepee::server::response& response)
  {
    worker.send_response(response);
  }

  void request_body_handler::read_buffer_from_worker(tepee::server::worker&         worker,
                                                     boost::beast::span<char>       buffer,
                                                     tepee::server::request_parser& parser,
                                                     boost::beast::error_code&      ec)
  {
    auto& parser_impl = static_cast<tepee::server::request_parser_impl&>(parser);
    worker.read_buffer(buffer, parser_impl, ec);
  }

  /* virtual */ void stream_request_body_handler::parse_request_body(std::shared_ptr<tepee::server::worker> worker,
                                                                     tepee::server::request_parser&         parser)
  {
    auto& parser_impl = static_cast<tepee::server::request_parser_impl&>(parser);

    boost::asio::post([this, worker, &parser_impl]() {
      using coroutine_t = boost::coroutines2::coroutine<std::tuple<char*, std::size_t, boost::beast::error_code>>;
      using pull_t      = coroutine_t::pull_type;
      using push_t      = coroutine_t::push_type;

      auto  stream     = std::make_unique<tepee::server::coroutine::stream<pull_t>>();
      auto& stream_ref = *stream.get();

      tepee::server::stream_request request(parser_impl.get().method(), parser_impl.get().target(), std::move(stream));

      std::array<char, 1024> buf;
      bool                   handle_request_done = false;
      push_t                 writer([this, worker, &stream_ref, &request, &handle_request_done](pull_t& in) {
        stream_ref.set_stream_puller(in);
        const auto& response = handle_(request);
        handle_request_done  = true;

        send_response_to_worker(*worker, response);
      });

      boost::beast::error_code ec;
      do
      {

        read_buffer_from_worker(*worker, boost::beast::span(buf.data(), buf.size()), parser_impl, ec);

        writer(std::make_tuple(buf.data(), buf.size() - parser_impl.get().body().size, ec));

        if(ec != boost::beast::http::error::need_buffer)
        {
          return;
        }
      } while(!parser_impl.is_done() && !handle_request_done);

    });
  }
} // namespace tepee::server