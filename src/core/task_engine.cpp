#include "task_engine.hpp"
#include <algorithm>

namespace JoseonRPG {

TaskId TaskEngine::s_nextId = 1;
std::vector<TaskEngine::TaskItem> TaskEngine::s_tasks;

TaskId TaskEngine::createTask(TaskFunc func, int priority) {
    if (!func) return 0;
    TaskId id = s_nextId++;
    s_tasks.push_back({id, priority, std::move(func), true});
    
    // Sort tasks by priority (higher priority first)
    std::stable_sort(s_tasks.begin(), s_tasks.end(), [](const TaskItem& a, const TaskItem& b) {
        return a.priority > b.priority;
    });

    return id;
}

void TaskEngine::destroyTask(TaskId id) {
    for (auto& item : s_tasks) {
        if (item.id == id) {
            item.active = false;
            break;
        }
    }
}

bool TaskEngine::isTaskRunning(TaskId id) {
    for (const auto& item : s_tasks) {
        if (item.id == id && item.active) {
            return true;
        }
    }
    return false;
}

void TaskEngine::clearAllTasks() {
    s_tasks.clear();
}

void TaskEngine::update(float dt) {
    // Process tasks
    for (size_t i = 0; i < s_tasks.size(); ++i) {
        if (!s_tasks[i].active) continue;
        
        TaskStatus status = s_tasks[i].func(dt);
        if (status == TaskStatus::Finished) {
            s_tasks[i].active = false;
        }
    }

    // Clean up finished tasks
    s_tasks.erase(
        std::remove_if(s_tasks.begin(), s_tasks.end(), [](const TaskItem& item) {
            return !item.active;
        }),
        s_tasks.end()
    );
}

TaskId TaskEngine::delay(float seconds, std::function<void()> onComplete) {
    struct DelayState {
        float elapsed = 0.0f;
        float duration = 0.0f;
        std::function<void()> cb;
    };

    auto state = std::make_shared<DelayState>();
    state->elapsed = 0.0f;
    state->duration = seconds;
    state->cb = std::move(onComplete);

    return createTask([state](float dt) -> TaskStatus {
        state->elapsed += dt;
        if (state->elapsed >= state->duration) {
            if (state->cb) state->cb();
            return TaskStatus::Finished;
        }
        return TaskStatus::Running;
    });
}

TaskId TaskEngine::slideValue(float& currentVal, float targetVal, float speed, std::function<void()> onComplete) {
    struct SlideState {
        float* valPtr = nullptr;
        float target = 0.0f;
        float spd = 0.0f;
        std::function<void()> cb;
    };

    auto state = std::make_shared<SlideState>();
    state->valPtr = &currentVal;
    state->target = targetVal;
    state->spd = speed;
    state->cb = std::move(onComplete);

    return createTask([state](float dt) -> TaskStatus {
        if (!state->valPtr) return TaskStatus::Finished;
        
        float diff = state->target - *(state->valPtr);
        float step = state->spd * dt;

        if (std::abs(diff) <= step) {
            *(state->valPtr) = state->target;
            if (state->cb) state->cb();
            return TaskStatus::Finished;
        } else {
            if (diff > 0.0f) *(state->valPtr) += step;
            else *(state->valPtr) -= step;
            return TaskStatus::Running;
        }
    });
}

TaskId TaskEngine::flash(int totalFlashes, float interval, std::function<void(bool visible)> onToggle, std::function<void()> onComplete) {
    struct FlashState {
        int flashesLeft = 0;
        float interval = 0.0f;
        float timer = 0.0f;
        bool visible = true;
        std::function<void(bool)> toggleCb;
        std::function<void()> completeCb;
    };

    auto state = std::make_shared<FlashState>();
    state->flashesLeft = totalFlashes * 2; // on + off per flash
    state->interval = interval;
    state->timer = 0.0f;
    state->visible = true;
    state->toggleCb = std::move(onToggle);
    state->completeCb = std::move(onComplete);

    return createTask([state](float dt) -> TaskStatus {
        state->timer += dt;
        if (state->timer >= state->interval) {
            state->timer = 0.0f;
            state->visible = !state->visible;
            if (state->toggleCb) state->toggleCb(state->visible);
            state->flashesLeft--;

            if (state->flashesLeft <= 0) {
                if (state->toggleCb) state->toggleCb(true); // reset visible
                if (state->completeCb) state->completeCb();
                return TaskStatus::Finished;
            }
        }
        return TaskStatus::Running;
    });
}

} // namespace JoseonRPG
