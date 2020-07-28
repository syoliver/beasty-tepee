cmake_minimum_required(VERSION 3.8)
project(Beasty-Tepee)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 17)


set(
  public_headers
  
  include/public/beasty-tepee/server.hpp
  include/public/beasty-tepee/response.hpp
  include/public/beasty-tepee/request_header_handler.hpp
  include/public/beasty-tepee/request_body_handler.hpp
)
set(
  private_headers

  include/private/stream_request.hpp
  include/private/worker.hpp
  include/private/server_impl.hpp
  include/private/fields_alloc.hpp
)

add_library(
  beasty-tepee STATIC
  
  src/server.cpp
  src/server_impl.cpp
  src/worker.cpp
  src/request_header_handler.cpp

  ${public_headers}
  ${private_headers}
)


target_include_directories(
  beasty-tepee

PRIVATE
  include/private

PUBLIC
  include/public
)

target_compile_definitions(
  beasty-tepee
PRIVATE
  _HAS_DEPRECATED_RESULT_OF=1
  _WIN32_WINNT=0x0601
  BOOST_BEAST_USE_STD_STRING_VIEW
)

target_link_libraries(beasty-tepee ${CONAN_LIBS})

add_subdirectory(examples)