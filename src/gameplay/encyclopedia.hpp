#pragma once
#include "../core/types.hpp"
#include <string>
#include <vector>
#include <array>
#include <string_view>

namespace JoseonRPG {

enum class DiscoveryStatus : uint8_t {
    Unseen,   // 아직 마주치지 못함 (???)
    Seen,     // 전투 중 조우함 (이름/외형 실루엣 확인)
    Captured  // 계약 완료 (상세 스탯, 설화 배경, 기술 전체 해금)
};

struct EncyclopediaEntry {
    int number = 0;
    std::string id;
    std::string nameKo;
    std::string nameEn;
    YokaiGrade baseGrade = YokaiGrade::Grade1;
    Element element = Element::Physical;
    std::string origin;
    std::string lore;
    DiscoveryStatus status = DiscoveryStatus::Unseen;
};

class Encyclopedia {
public:
    Encyclopedia();

    void registerTemplate(int number, std::string id, std::string nameKo, std::string nameEn,
                          YokaiGrade baseGrade, Element element, std::string origin, std::string lore);

    void markSeen(std::string_view id);
    void markCaptured(std::string_view id);
    void markSeen(int number);
    void markCaptured(int number);

    const EncyclopediaEntry* getEntry(int number) const;
    const EncyclopediaEntry* getEntryById(std::string_view id) const;

    size_t getTotalEntries() const { return m_entries.size(); }
    int getSeenCount() const;
    int getCapturedCount() const;
    float getCompletionRate() const;

    const std::vector<EncyclopediaEntry>& getAllEntries() const { return m_entries; }

private:
    std::vector<EncyclopediaEntry> m_entries;
};

} // namespace JoseonRPG
