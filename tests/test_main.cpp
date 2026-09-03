#include <iostream>

bool runBattleSystemTests();
bool runStatusEffectTests();
bool runArtifactSystemTests();
bool runEncyclopediaAndGrowthTests();
bool runVerticalSliceTests();
bool runFullContentTests();
bool runSaveSystemAndTaskEngineTests();
bool runPhase7SoundAndTraitsTests();

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " 108: EUMYANG GYEONMUNROK - UNIT TESTS " << std::endl;
    std::cout << "========================================" << std::endl;

    bool passed = true;
    if (!runBattleSystemTests()) passed = false;
    if (!runStatusEffectTests()) passed = false;
    if (!runArtifactSystemTests()) passed = false;
    if (!runEncyclopediaAndGrowthTests()) passed = false;
    if (!runVerticalSliceTests()) passed = false;
    if (!runFullContentTests()) passed = false;
    if (!runSaveSystemAndTaskEngineTests()) passed = false;
    if (!runPhase7SoundAndTraitsTests()) passed = false;

    std::cout << "----------------------------------------" << std::endl;
    if (passed) {
        std::cout << "[ALL TESTS PASSED SUCCESSFULLY!]" << std::endl;
        return 0;
    } else {
        std::cerr << "[TEST FAILURES DETECTED!]" << std::endl;
        return 1;
    }
}
