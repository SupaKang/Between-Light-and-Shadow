#pragma once
#include "../core/types.hpp"
#include "../core/renderer.hpp"
#include <string>
#include <vector>
#include <functional>

namespace JoseonRPG {

class DialogueBox {
public:
    DialogueBox();

    void startDialogue(const std::string& speaker, const std::vector<std::string>& lines, std::function<void()> onComplete = nullptr);
    void startDialogueWithOptions(const std::string& speaker, const std::vector<std::string>& lines, const std::vector<std::string>& options, std::function<void(int optionIndex)> onSelectOption);
    void update(float dt);
    void render(Renderer& renderer);
    bool handleInput(); // returns true if input was handled

    bool isActive() const { return m_active; }
    void close();

    void setSpeed(float charsPerSec) { m_charsPerSecond = charsPerSec; }

private:
    bool m_active = false;
    std::string m_speaker;
    std::vector<std::string> m_lines;
    size_t m_currentLineIndex = 0;
    
    // Typewriter effect state
    float m_charsPerSecond = 35.0f;
    float m_charProgress = 0.0f;
    bool m_isLineFullyRevealed = false;

    // Prompt indicator blink
    float m_promptBlinkTimer = 0.0f;
    bool m_promptVisible = true;

    // Branching options state
    std::vector<std::string> m_options;
    int m_selectedOptionIndex = 0;
    bool m_isChoosingOption = false;

    std::function<void()> m_onCompleteCallback = nullptr;
    std::function<void(int optionIndex)> m_onOptionSelectedCallback = nullptr;
};

} // namespace JoseonRPG
