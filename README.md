# CMake-Driven C++ Training & Python Inference Project

This project demonstrates a fully automated workflow where CMake manages both the C++ and Python components.

## Workflow
1.  **`cmake ..`**: Configures the project.
2.  **`make`**:
    *   Triggers the `setup.sh` script to create a Python virtual environment (`build/cpp_venv`) and install dependencies from `requirements.txt`.
    *   Compiles the C++ `cpp_trainer` application.
    *   Copies the `inference.py` script into the build directory.
3.  **`./cpp_trainer`**:
    *   The C++ program defines and trains a simple linear regression model.
    *   It saves the trained model as `model_from_cpp.pt` in the build directory.
    *   The C++ program then executes the `inference.py` script.
    *   The Python script loads `model_from_cpp.pt`, runs inference, and prints the result, confirming the model works.

## Prerequisites
- A C++/C compiler (g++, clang)
- CMake (version 3.12+)
- Python 3.7+ and `python3-venv` package.

## Setup & Run

### 1. Get LibTorch
You must have the LibTorch C++ library.
- Go to [pytorch.org/get-started/locally/](https://pytorch.org/get-started/locally/).
- Select `LibTorch`, your OS, `C++`, and `CPU`/`CUDA`.
- Download and unzip it.

### 2. Configure CMakeLists.txt
Open `CMakeLists.txt` and **update the path to your LibTorch installation**:
```cmake
# --- IMPORTANT: UPDATE THIS PATH ---
list(APPEND CMAKE_PREFIX_PATH /path/to/your/libtorch)