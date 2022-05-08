from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class PatternStreamsConan(ConanFile):
    name = "PatternStreams"
    version = "1.0.0"

    # Optional metadata
    license = "MIT"
    author = "Gabriel Pizarro (gpizarro@javaman.net)"
    url = "https://github.com/TheNewJavaman/PatternStreams"
    description = "Simplifies searching and patching runtime memory in C++ using syntax similar to Java streams"
    topics = ("windows", "assembly", "asm")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["PatternStreams"]
