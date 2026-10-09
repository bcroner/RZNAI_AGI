#ifndef I_EQUATION_ENGINE_H
#define I_EQUATION_ENGINE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <system_error>

namespace EverythingEquation {

// Represents input parameters passed directly into computational solver engines
struct DatasetPayload {
    std::string equation_string;
    std::map<std::string, double> variable_bindings;
    std::vector<double> continuous_matrix_data;
};

// Represents structured numerical computation output matrices
struct EvaluationResult {
    bool is_valid_solution = false;
    double primary_scalar_output = 0.0;
    std::vector<double> computed_tensor_array;
    std::error_code execution_error_status;
};

class IEquationEngine {
public:
    virtual ~IEquationEngine() = default;

    // Direct interface to identify active processing hardware signature profiles
    virtual std::string GetBackendEngineIdentifier() const = 0;

    // Primary mathematical execution entry point across structural matrices
    virtual EvaluationResult EvaluateSystem(const DatasetPayload& payload) = 0;
};

} // namespace EverythingEquation

#endif // I_EQUATION_ENGINE_H
