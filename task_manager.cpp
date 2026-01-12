#include "TaskManager.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

TaskManager::TaskManager(const std::string& storagePath)
    : m_storagePath(storagePath), m_nextId(1) {
    load();
}

TaskManager::~TaskManager() {
    save();
}

void TaskManager::ensureDataDir() const {
    std::filesystem::path p(m_storagePath);
    auto dir = p.parent_path();
    if (dir.empty()) return;
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
}

void TaskManager::load() {
    m_tasks.clear();
    std::ifstream in(m_storagePath);
    if (!in.is_open()) {
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            Task t = Task::fromCSV(line);
            m_tasks.push_back(t);
            if (t.id() >= m_nextId) m_nextId = t.id() + 1;
        } catch (...) {
        }
    }
}

void TaskManager::save() const {
    ensureDataDir();
    std::ofstream out(m_storagePath, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Warning: could not open storage file for saving: " << m_storagePath << "\n";
        return;
    }
    for (const auto& t : m_tasks) {
        out << t.toCSV() << '\n';
    }
}

Task TaskManager::addTask(const std::string& title,
                          const std::string& description,
                          int priority,
                          const std::string& deadline) {
    Task t(m_nextId++, title, description, priority, deadline, false);
    m_tasks.push_back(t);
    save();
    return t;
}

std::vector<Task> TaskManager::listAll() const {
    return m_tasks;
}

bool TaskManager::markDone(int id, bool done) {
    for (auto& t : m_tasks) {
        if (t.id() == id) {
            t.setDone(done);
            save();
            return true;
        }
    }
    return false;
}

bool TaskManager::removeTask(int id) {
    for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
        if (it->id() == id) {
            m_tasks.erase(it);
            save();
            return true;
        }
    }
    return false;
}

Task* TaskManager::findTask(int id) {
    for (auto& t : m_tasks) {
        if (t.id() == id) return &t;
    }
    return nullptr;
}

bool TaskManager::editTask(int id, const std::string& title,
                           const std::string& description,
                           int priority, const std::string& deadline) {
    Task* t = findTask(id);
    if (!t) return false;
    if (!title.empty()) t->setTitle(title);
    if (!description.empty()) t->setDescription(description);
    if (priority >= 0) t->setPriority(priority);
    if (!deadline.empty()) t->setDeadline(deadline);
    save();
    return true;
}
