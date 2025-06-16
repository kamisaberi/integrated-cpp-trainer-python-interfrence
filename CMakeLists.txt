cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(CppTrainerProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- IMPORTANT: UPDATE THIS PATH ---
# Point this to the location of your downloaded LibTorch library.
# The directory should contain include/ and lib/ subdirectories.
list(APPEND CMAKE_PREFIX_PATH /home/kami/libs/cpp/libtorch/)

# Find LibTorch package
find_package(Torch REQUIRED)

# ======================================================================
# STAGE 1: PYTHON ENVIRONMENT SETUP
# ======================================================================

set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpp_venv")
set(PYTHON_EXECUTABLE "${VENV_DIR}/bin/python3")

add_custom_target(
        setup_python_env ALL
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/setup.sh" "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Setting up Python virtual environment and dependencies..."
        VERBATIM
)

# ======================================================================
# STAGE 2: C++ APPLICATION BUILD
# ======================================================================

add_executable(cpp_trainer cpp_trainer.cpp)

# Make sure Python setup completes before compiling C++
add_dependencies(cpp_trainer setup_python_env)

# Link against LibTorch
target_link_libraries(cpp_trainer PRIVATE "${TORCH_LIBRARIES}")

# Copy the Python converter script to the build directory so the C++
# executable can find and run it easily.
add_custom_command(
        TARGET cpp_trainer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/converter.py"
        "$<TARGET_FILE_DIR:cpp_trainer>/converter.py"
        COMMENT "Copying Python converter script to build directory"
)

add_custom_command(
        TARGET cpp_trainer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/inference.py"
        "$<TARGET_FILE_DIR:cpp_trainer>/inference.py"
        COMMENT "Copying Python inference script to build directory"
)