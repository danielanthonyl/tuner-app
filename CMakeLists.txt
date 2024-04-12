cmake_minimum_required(VERSION 3.0)
project("tuner")

# Add Kiss FFT source files
set(KISS_FFT_SOURCES kiss_fft.c)  # Adjust filename if different
set(KISSFFT_BUILD_TESTS OFF)

# Include Kiss FFT directory in your project's include paths

# Add Kiss FFT sources to your project's target
add_executable(tuner main.cpp "C:/Users/dhans/code/c/tuner/dependencies/kissfft-131.1.0/kiss_fft.c")