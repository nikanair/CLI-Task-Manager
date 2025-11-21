#pragma once
#include "Task.h"
#include <vector>
#include <string>

class TaskManager {
public:
    explicit TaskManager(const std::string& storagePath = "data/tasks.csv");
    ~TaskManager();

    void load();
    void save() const;

    Task addTask(const std::string& title,
                 const std::string& description,
                 int priority,
                 const std::string& deadline);

    std::vector<Task> listAll() const;
    bool markDone(int id, bool done = true);
    bool removeTask(int id);
    Task* findTask(int id);
    bool editTask(int id, const std::string& title,
                  const std::string& description,
                  int priority, const std::string& deadline);

private:
    std::vector<Task> m_tasks;
    std::string m_storagePath;
    int m_nextId;
    void ensureDataDir() const;
};
