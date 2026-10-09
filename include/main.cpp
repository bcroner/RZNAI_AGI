#include "AgiBroker.h"
#include <iostream>

int main() {
    std::cout << "=================================================================\n"
              << "    RZNAI AGI Node: Automated Computational Interface Loop       \n"
              << "=================================================================\n\n";

    // 1. Instantiation & Setup Configuration
    auto broker = std::make_unique<RZNAI::AgiBroker>();
    size_t target_matrix_dimension = 2048; // Allocation scope token signature size
    
    if (!broker->ArmCoreInfrastructure(target_matrix_dimension)) {
        std::cerr << "[Fatal Exception]: Hardware initialization breakdown.\n";
        return 1;
    }

    // 2. Create sample payload context frame
    RZNAI::ContextFrame mock_frame{
        904571,                          // Unique execution frame token ID
        {0.115f, -0.482f, 0.891f},      // Simulated latent-space embedding matrix array
        "Context Domain: Systems Engineering; Active Logic Gates Enabled;"
    };

    // 3. Dispatch data down the execution lane asynchronously
    std::cout << "\n[Streaming Pipeline]: Enqueueing context task structure sequence...\n";
    std::future<RZNAI::InferenceOutput> pending_inference = broker->RouteInferenceTaskAsync(mock_frame);

    // Main thread remains entirely free to stream other operations or handle socket updates here...
    std::cout << "[Main Orchestrator Loop]: Continuing background network processing concurrently.\n";

    // 4. Await and harvest the async output matrix
    RZNAI::InferenceOutput result = pending_inference.get();

    std::cout << "\n[Execution Result Compiled Successfully]:\n"
              << "  -> Success Status: " << (result.execution_success ? "TRUE" : "FALSE") << "\n"
              << "  -> Execution Log:  " << result.actionable_response_text << "\n";

    if (!result.computed_confidence_matrix.empty()) {
        std::cout << "  -> Primary Activation Confidence: " << result.computed_confidence_matrix[0] * 100.0f << "%\n";
    }

    std::cout << "\n================= RZNAI Node Processing Cycle Concluded ===============\n";
    return 0;
}
