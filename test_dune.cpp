// Test compilation of DUNE clockface with background cycling
#include "components/clockwise_hub75/dune_Clockface.h"
#include "components/clockwise_hub75/dune_assets.h"

// Simple test to verify our implementation compiles
int main() {
    dune::Clockface clockface(nullptr, nullptr);
    // This should compile successfully with our background cycling implementation
    return 0;
}