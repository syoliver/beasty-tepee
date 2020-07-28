#include <cstdlib>
#include <iostream>
#include <boost/url/url.hpp>

int main() {
  const boost::urls::url conan_url("https://conan.io/center/");

  if( conan_url.scheme() == "https" && conan_url.encoded_host() == "conan.io" && conan_url.encoded_path() == "/center/")
  {
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
