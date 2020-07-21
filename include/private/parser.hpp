#pragma once

#include "fields_alloc.hpp"
#include <beasty-tepee/request_parser.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/buffer_body.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>

namespace tepee::server
{
    using alloc_t = fields_alloc<char>;
    using request_body_t = boost::beast::http::buffer_body;
    using response_t = boost::beast::http::response<boost::beast::http::string_body, boost::beast::http::basic_fields<alloc_t>>;

    
    using request_parser_t = boost::beast::http::request_parser<request_body_t, alloc_t>;


    struct request_parser_impl : public request_parser_t, public request_parser
    {
      using request_parser_t::request_parser_t;
    };
}