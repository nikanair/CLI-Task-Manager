#pragma once
#include <string>

class Task {
public:
    Task() = default;
    Task(int id, std::string title, std::string description,
         int priority, std::string deadline, bool done = false);

    int id() const;
    const std::string& title() const;
    const std::string& description() const;
    int priority() const;
    const std::string& deadline() const;
    bool done() const;

    void setTitle(const std::string& t);
    void setDescription(const std::string& d);
    void setPriority(int p);
    void setDeadline(const std::string& d);
    void setDone(bool val);

    std::string toCSV() const;
    static Task fromCSV(const std::string& line);

private:
    int m_id{0};
    std::string m_title;
    std::string m_description;
    int m_priority{0};
    std::string m_deadline;
    bool m_done{false};
};
