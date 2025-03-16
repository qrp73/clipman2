cmake_minimum_required(VERSION 3.10)
project(clipman2 C)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Enable strict compilation flags
add_compile_options(
    -Wall          # Enable all warning messages
    -Wextra        # Enable extra warnings not covered by -Wall
    -Wpedantic     # Enable strict ISO C and C++ compliance warnings
    -Werror        # Treat all warnings as errors
)

# Add executable file
add_executable(clipman2 clipman2.c)

# Set installation path
install(TARGETS clipman2 DESTINATION /usr/bin)
