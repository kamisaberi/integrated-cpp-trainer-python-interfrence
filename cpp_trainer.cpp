// trainer.cpp (with Python launcher)
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <cstdlib> // <-- The header for the system() command
#include <torch/torch.h>

// The model we will train in C++
struct Net : torch::nn::Module {
    torch::nn::Linear fc1{nullptr}, fc2{nullptr};
    Net() {
        fc1 = register_module("fc1", torch::nn::Linear(1, 64));
        fc2 = register_module("fc2", torch::nn::Linear(64, 1));
    }
    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(fc1(x));
        x = fc2(x);
        return x;
    }
};

// Helper function to save the raw bytes of the model's parameters (no changes here)
void save_parameters(const torch::OrderedDict<std::string, torch::Tensor>& params, const std::string& file_path) {
    std::ofstream out(file_path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot open file for writing: " + file_path);
    }
    uint32_t num_params = params.size();
    out.write(reinterpret_cast<const char*>(&num_params), sizeof(num_params));

    for (const auto& pair : params) {
        uint32_t name_len = pair.key().length();
        out.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        out.write(pair.key().c_str(), name_len);

        torch::Tensor tensor = pair.value().to(torch::kCPU).contiguous();

        uint32_t dims = tensor.dim();
        out.write(reinterpret_cast<const char*>(&dims), sizeof(dims));
        out.write(reinterpret_cast<const char*>(tensor.sizes().data()), dims * sizeof(int64_t));
        out.write(static_cast<const char*>(tensor.data_ptr()), tensor.nbytes());
    }
    out.close();
}

int main() {
    std::cout << "--- Step 1: C++ Trainer ---" << std::endl;

    auto model = std::make_shared<Net>();
    model->train();

    // Dummy training loop
    torch::Tensor inputs = torch::randn({100, 1});
    torch::Tensor labels = inputs * 5 - 2; // Target function
    torch::optim::Adam optimizer(model->parameters(), 0.01);

    std::cout << "Training the model..." << std::endl;
    for (int epoch = 0; epoch < 100; ++epoch) {
        optimizer.zero_grad();
        auto output = model->forward(inputs);
        auto loss = torch::mse_loss(output, labels);
        loss.backward();
        optimizer.step();
    }
    std::cout << "Training complete." << std::endl;

    // Save the trained weights to a file
    const std::string WEIGHTS_FILE = "cpp_trained_weights.bin";
    try {
        save_parameters(model->named_parameters(), WEIGHTS_FILE);
        std::cout << "✅ Success: Raw weights saved to '" << WEIGHTS_FILE << "'" << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "❌ Error saving weights: " << e.what() << std::endl;
        return 1;
    }

    // ===================================================================
    // NEW SECTION: Automatically launch the Python converter script
    // ===================================================================
    std::cout << "\n--- C++ is now launching the Python converter script ---" << std::endl;

    // We assume 'python3' is in the system's PATH and 'converter.py' is in the same directory.
    const char* python_command = "./cpp_venv/bin/python3 converter.py";
    int return_code = system(python_command);

    if (return_code != 0) {
        std::cerr << "\n❌ Error: Python script failed to execute correctly. Exit code: " << return_code << std::endl;
        return 1;
    }



    // We assume 'python3' is in the system's PATH and 'converter.py' is in the same directory.
    python_command = "./cpp_venv/bin/python3 inference.py";
    return_code = system(python_command);

    if (return_code != 0) {
        std::cerr << "\n❌ Error: Python script failed to execute correctly. Exit code: " << return_code << std::endl;
        return 1;
    }


    std::cout << "\n--- C++ application has completed the entire workflow ---" << std::endl;
    // The final `independent_model.pt` file should now exist.





    return 0;
}