from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
import os
import glob
import shutil

class BoostUrl(ConanFile):
    name = "boost.url"
    homepage = "https://github.com/CPPAlliance/url"
    license = ("BSL-1.0")
    url = "https://github.com/conan-io/conan-center-index"
    description = "Boost url"
    topics = ("conan", "boost", "url", "uri")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "standalone": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "standalone": False

    }
    exports_sources = ['patches/*', "CMakeLists.txt"]
    generators = "cmake", "cmake_find_package"

    _cmake = None

    @property
    def _source_subfolder(self):
        return "source_subfolder"

    @property
    def _build_subfolder(self):
        return "build_subfolder"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

        minimal_cpp_standard = "11"
        if self.settings.compiler.cppstd:
            tools.check_min_cppstd(self, minimal_cpp_standard)


    def source(self):
        tools.get(**self.conan_data["sources"][self.version])
        extracted_dir = "url-{commit}".format(
          commit = os.path.splitext(os.path.basename(self.conan_data["sources"][self.version]["url"]))[0],
        )
        os.rename(extracted_dir, self._source_subfolder)

    def _configure_cmake(self):
        if not self._cmake:
            self._cmake = CMake(self)

        self._cmake.definitions["BOOST_URL_BUILD_TESTS"] = False
        self._cmake.definitions["BOOST_URL_BUILD_EXAMPLES"] = False

        self._cmake.configure(build_folder=self._build_subfolder)
        return self._cmake

    def build(self):
        for patch in self.conan_data["patches"].get(self.version, []):
            tools.patch(**patch)
        cmake = self._configure_cmake()
        cmake.build()

    def requirements(self):
        self.requires("boost/1.73.0")


    def package(self):
        self.copy("COPYING", dst="licenses", src=self._source_subfolder)
        self.copy("LICENSE*", dst="licenses", src=self._source_subfolder)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
