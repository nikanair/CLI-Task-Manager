#include "Task.h"
#include "Utils.h"
#include <sstream>
#include <stdexcept>

Task::Task(int id, std::string title, std::string description,
           int priority, std::string deadline, bool done)
    : m_id(id),
      m_title(std::move(title)),
      m_description(std::move(description)),
      m_priority(priority),
      m_deadline(std::move(deadline)),
      m_done(done) {}

int Task::id() const { return m_id; }
const std::string& Task::title() const { return m_title; }
const std::string& Task::description() const { return m_description; }
int Task::priority() const { return m_priority; }
const std::string& Task::deadline() const { return m_deadline; }
bool Task::done() const { return m_done; }

void Task::setTitle(const std::string& t) { m_title = t; }
void Task::setDescription(const std::string& d) { m_description = d; }
void Task::setPriority(int p) { m_priority = p; }
void Task::setDeadline(const std::string& d) { m_deadline = d; }
void Task::setDone(bool val) { m_done = val; }

std::string Task::toCSV() const {
    std::ostringstream oss;
    oss << m_id << ','
        << Utils::escapeCSV(m_title) << ','
        << Utils::escapeCSV(m_description) << ','
        << m_priority << ','
        << Utils::escapeCSV(m_deadline) << ','
        << (m_done ? "1" : "0");
    return oss.str();
}

Task Task::fromCSV(const std::string& line) {
    auto parts = Utils::splitCSV(line);

    if (parts.size() != 6) {
        throw std::runtime_error("Invalid CSV line: expected 6 fields, got " + std::to_string(parts.size()));
    }

    int id = Utils::stoiSafe(parts[0], "id");
    std::string title = Utils::unescapeCSV(parts[1]);
    std::string description = Utils::unescapeCSV(parts[2]);
    int priority = Utils::stoiSafe(parts[3], "priority");
    std::string deadline = Utils::unescapeCSV(parts[4]);
    bool done = (parts[5] == "1");

    return Task{id, title, description, priority, deadline, done};
}
