#pragma once

#include <boost/beast/http/error.hpp>
#include <boost/beast/http/verb.hpp>
#include <algorithm>
#include <deque>
#include <memory>
#include <string_view>
#include <tuple>

namespace tepee::server
{
  class stream
  {
   public:
    using streamsize = std::size_t;

    virtual streamsize readsome(char* s, streamsize n, boost::beast::error_code& ec) = 0;
  };

  class stream_request
  {
   public:
    using stream_ptr = std::unique_ptr<stream>;

   public:
    stream_request(boost::beast::http::verb method, std::string_view target, stream_ptr stream)
        : method_(method)
        , target_(target)
        , body_(std::move(stream))
    {
    }

    boost::beast::http::verb method() const { return method_; }
    std::string_view         target() const { return target_; }
    stream&                  body_stream() const { return *body_.get(); }

   private:
    boost::beast::http::verb method_;
    std::string_view         target_;
    stream_ptr               body_;
  };
} // namespace tepee::server

namespace tepee::server::coroutine
{
  template <typename Pull, typename Allocator = std::allocator<char>>
  class stream : public tepee::server::stream
  {
   public:
    using streamsize = tepee::server::stream::streamsize;
    stream(Allocator alloc = Allocator()) : pull_(nullptr), buffer_(alloc), offset_(0), eof_(false), first_step_(true) {}

    virtual streamsize readsome(char* s, streamsize n, boost::beast::error_code& ec) override
    {
      if(offset_ < buffer_.size())
      {
        streamsize readn = std::min(n, buffer_.size() - offset_);
        std::memcpy(&buffer_[0] + offset_, s, readn);
        offset_ += readn;
        return readn;
      }

      if(pull_ == nullptr || eof_)
      {
        ec = boost::beast::http::error::end_of_stream;
        return 0;
      }

      char*       buf  = nullptr;
      std::size_t size = 0;

      if(!first_step_)
      {
        (*pull_)(); // context switch to parser in case of buffer if not ended
      }
      first_step_             = false;
      std::tie(buf, size, ec) = pull_->get();

      if(!ec)
      {
        eof_ = true;
      }
      else if(ec != boost::beast::http::error::need_buffer)
      {
        return 0;
      }

      if(size <= n)
      {
        std::memcpy(s, buf, size);
        return size;
      }

      std::memcpy(s, buf, n);

      offset_ = 0;
      buffer_.resize(size - n);
      std::memcpy(&buffer_[0], buf + n, size - n);

      return n;
    }

    void set_stream_puller(Pull& pull) { pull_ = &pull; }

   private:
    Pull*                        pull_;
    std::vector<char, Allocator> buffer_;
    std::size_t                  offset_;
    bool                         eof_;
    bool                         first_step_;
  };

} // namespace tepee::server::coroutine