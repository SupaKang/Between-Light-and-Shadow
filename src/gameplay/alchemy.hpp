#pragma once
#include "../core/types.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

class Party;

enum class AlchemyRecipeId {
    VitalityDecoction, // 벽사체력탕: 파티원 1마리 HP +120 회복 (비용: 50냥)
    SpiritQiPill,      // 청심영력환: 파티원 1마리 Qi +60 회복 (비용: 60냥)
    CenturyGinsengPill // 백년산삼 천지환: 파티 전원 HP/Qi 100% 완치 및 상태이상 해제 (비용: 300냥)
};

struct AlchemyRecipe {
    AlchemyRecipeId id;
    std::string name;
    std::string description;
    int costMoney;
};

class AlchemySystem {
public:
    static std::vector<AlchemyRecipe> getAvailableRecipes();
    static bool brewPotion(AlchemyRecipeId recipeId, Party& party, int& playerMoney, std::string& outMessage);
};

} // namespace JoseonRPG
