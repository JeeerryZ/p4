#pragma once

#include "../file_tab.h"
#include "../third_party/crow/crow_all.h"
#include <string>

// Converts parsed WVFStudy results to a Crow JSON response value.
crow::json::wvalue buildResponse(const WVFStudy &study,
                                 const std::string &rawOutput);

// Builds a JSON error object.
crow::json::wvalue buildError(const std::string &message);
