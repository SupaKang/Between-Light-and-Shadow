#pragma once
#include "yokai.hpp"
#include <vector>
#include <memory>

namespace JoseonRPG {

class Party {
public:
    Party() = default;

    bool addYokai(const Yokai& yokai);
    bool removeYokai(size_t index);
    bool swapYokai(size_t indexA, size_t indexB);

    Yokai* getActiveYokai();
    const Yokai* getActiveYokai() const;

    Yokai* getYokai(size_t index);
    const Yokai* getYokai(size_t index) const;

    size_t getSize() const { return m_members.size(); }
    bool isAllFainted() const;
    void healAll();

private:
    std::vector<Yokai> m_members;
};

} // namespace JoseonRPG
