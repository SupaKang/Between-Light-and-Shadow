#include "alchemy.hpp"
#include "../gameplay/party.hpp"
#include "../audio/audio_engine.hpp"

namespace JoseonRPG {

std::vector<AlchemyRecipe> AlchemySystem::getAvailableRecipes() {
    return {
        {AlchemyRecipeId::VitalityDecoction, "벽사체력탕", "붉은 산초와 지황으로 조제. 파티 선두 HP +120 회복", 50},
        {AlchemyRecipeId::SpiritQiPill, "청심영력환", "맑은 감초와 석창포로 조제. 파티 선두 Qi +60 회복", 60},
        {AlchemyRecipeId::CenturyGinsengPill, "백년산삼 천지환", "심마니의 백년 산삼으로 조제. 파티 전원 완치 및 상태이상 해제", 300}
    };
}

bool AlchemySystem::brewPotion(AlchemyRecipeId recipeId, Party& party, int& playerMoney, std::string& outMessage) {
    auto recipes = getAvailableRecipes();
    const AlchemyRecipe* selected = nullptr;
    for (const auto& r : recipes) {
        if (r.id == recipeId) {
            selected = &r;
            break;
        }
    }

    if (!selected) {
        outMessage = "존재하지 않는 비방입니다.";
        return false;
    }

    if (playerMoney < selected->costMoney) {
        AudioEngine::playSfx(SfxId::MenuCancel);
        outMessage = "엽전이 부족합니다! (" + std::to_string(selected->costMoney) + "냥 필요)";
        return false;
    }

    playerMoney -= selected->costMoney;
    AudioEngine::playSfx(SfxId::HitMagic);

    switch (recipeId) {
        case AlchemyRecipeId::VitalityDecoction: {
            Yokai* lead = party.getActiveYokai();
            if (lead) {
                lead->healHp(120);
                outMessage = "★ [벽사체력탕] 조제 완료! " + lead->getName() + "의 체력 120 회복! ★";
            }
            break;
        }
        case AlchemyRecipeId::SpiritQiPill: {
            Yokai* lead = party.getActiveYokai();
            if (lead) {
                lead->restoreQi(60);
                outMessage = "★ [청심영력환] 조제 완료! " + lead->getName() + "의 영력 60 회복! ★";
            }
            break;
        }
        case AlchemyRecipeId::CenturyGinsengPill: {
            party.healAll();
            outMessage = "★ [백년산삼 천지환] 조제 완료! 파티 전원 체력/영력 완치 및 상태이상 해제! ★";
            break;
        }
    }

    return true;
}

} // namespace JoseonRPG
