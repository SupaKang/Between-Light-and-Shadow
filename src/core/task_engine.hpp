#pragma once
#include "types.hpp"
#include <functional>
#include <vector>
#include <memory>
#include <cstdint>

namespace JoseonRPG {

using TaskId = uint32_t;

enum class TaskStatus {
    Running,
    Finished
};

class TaskEngine {
public:
    using TaskFunc = std::function<TaskStatus(float dt)>;

    static TaskId createTask(TaskFunc func, int priority = 0);
    static void destroyTask(TaskId id);
    static bool isTaskRunning(TaskId id);
    static void clearAllTasks();

    static void update(float dt);

    // Prebuilt helper tasks
    static TaskId delay(float seconds, std::function<void()> onComplete);
    static TaskId slideValue(float& currentVal, float targetVal, float speed, std::function<void()> onComplete = nullptr);
    static TaskId flash(int totalFlashes, float interval, std::function<void(bool visible)> onToggle, std::function<void()> onComplete = nullptr);

private:
    struct TaskItem {
        TaskId id = 0;
        int priority = 0;
        TaskFunc func;
        bool active = true;
    };

    static TaskId s_nextId;
    static std::vector<TaskItem> s_tasks;
};

} // namespace JoseonRPG
