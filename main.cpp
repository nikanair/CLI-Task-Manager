#include "TaskManager.h"
#include "Utils.h"
#include <iostream>
#include <sstream>

void printHelp() {
    std::cout <<
    "TaskFlow - simple CLI task manager\n\n"
    "Commands:\n"
    "  add <title> [-d description] [-p priority] [-t deadline]    Add a task\n"
    "  list                                                        List all tasks\n"
    "  done <id>                                                   Mark task done\n"
    "  undone <id>                                                 Mark task not done\n"
    "  remove <id>                                                 Remove a task\n"
    "  edit <id> [-t title] [-d description] [-p priority] [-D deadline]  Edit fields\n"
    "  help                                                        Show this message\n"
    "  exit                                                        Exit\n"
    "\nExamples:\n  add \"Buy groceries\" -d \"milk, eggs\" -p 2 -t 2025-12-01\n  list\n  done 3\n";
}

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> out;
    std::istringstream iss(line);
    std::string token;
    while (iss >> std::quoted(token)) {
        out.push_back(token);
    }
    return out;
}

// Simple parser for add with flags (minimal)
int main(int argc, char** argv) {
    TaskManager mgr("data/tasks.csv");

    if (argc > 1) {
        // Allow single-command usage: e.g., taskflow add "title" -d "desc"
        std::ostringstream oss;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]).find(' ') != std::string::npos) {
                oss << '"' << argv[i] << "\" ";
            } else {
                oss << argv[i] << ' ';
            }
        }
        std::string built(oss.str());
        // fall through to interactive parser below using built string
        std::cout << "Single-run mode not implemented in this build. Launching interactive shell...\n";
    }

    printHelp();
    std::string line;
    while (true) {
        std::cout << "\n> ";
        if (!std::getline(std::cin, line)) break;
        auto trimmed = Utils::trim(line);
        if (trimmed.empty()) continue;

        std::istringstream iss(trimmed);
        std::string cmd;
        iss >> cmd;

        if (cmd == "help") {
            printHelp();
        } else if (cmd == "exit" || cmd == "quit") {
            break;
        } else if (cmd == "list") {
            auto tasks = mgr.listAll();
            if (tasks.empty()) {
                std::cout << "No tasks.\n";
                continue;
            }
            std::cout << "ID | P | Done | Title (deadline)\n";
            std::cout << "-------------------------------------------\n";
            for (const auto& t : tasks) {
                std::cout << t.id() << " | " << t.priority()
                          << " | " << (t.done() ? "x" : " ") << " | "
                          << t.title() << " (" << t.deadline() << ")\n";
                if (!t.description().empty()) {
                    std::cout << "    " << t.description() << '\n';
                }
            }
        } else if (cmd == "add") {
            // gather rest of line as title and flags
            std::string rest;
            std::getline(iss, rest);
            rest = Utils::trim(rest);
            std::string title;
            std::string description;
            int priority = 0;
            std::string deadline;

            // naive parsing: look for -d, -p, -t flags; title is before first flag
            size_t posFlag = std::string::npos;
            size_t posD = rest.find(" -d ");
            size_t posP = rest.find(" -p ");
            size_t posT = rest.find(" -t ");
            std::vector<size_t> flags;
            if (posD != std::string::npos) flags.push_back(posD);
            if (posP != std::string::npos) flags.push_back(posP);
            if (posT != std::string::npos) flags.push_back(posT);
            if (!flags.empty()) {
                posFlag = *std::min_element(flags.begin(), flags.end());
            }
            if (posFlag == std::string::npos) {
                title = rest;
            } else {
                title = Utils::trim(rest.substr(0, posFlag));
                std::string tail = rest.substr(posFlag);
                // parse tail tokens
                std::istringstream tails(tail);
                std::string flag;
                while (tails >> flag) {
                    if (flag == "-d") {
                        std::string val;
                        std::getline(tails, val, '-'); // read until next '-' (rough)
                        if (!val.empty()) {
                            // remove trailing possible '-'
                            if (val.back() == ' ') val.pop_back();
                            description = Utils::trim(val);
                        }
                    } else if (flag == "-p") {
                        std::string val;
                        if (!(tails >> val)) break;
                        priority = Utils::stoiSafe(val, 0);
                    } else if (flag == "-t") {
                        std::string val;
                        if (!(tails >> std::quoted(val))) {
                            // fallback: read token
                            tails >> val;
                        }
                        deadline = Utils::trim(val);
                    } else {
                        // unknown, skip
                    }
                }
            }
            if (title.empty()) {
                std::cout << "Title required. Usage: add <title> [-d description] [-p priority] [-t deadline]\n";
                continue;
            }
            Task t = mgr.addTask(title, description, priority, deadline);
            std::cout << "Added task ID " << t.id() << '\n';
        } else if (cmd == "done" || cmd == "undone") {
            int id;
            if (!(iss >> id)) {
                std::cout << "Usage: " << cmd << " <id>\n";
                continue;
            }
            bool val = (cmd == "done");
            if (mgr.markDone(id, val)) {
                std::cout << "Task " << id << (val ? " marked done.\n" : " marked not done.\n");
            } else {
                std::cout << "Task " << id << " not found.\n";
            }
        } else if (cmd == "remove") {
            int id;
            if (!(iss >> id)) {
                std::cout << "Usage: remove <id>\n";
                continue;
            }
            if (mgr.removeTask(id)) {
                std::cout << "Removed task " << id << '\n';
            } else {
                std::cout << "Task " << id << " not found.\n";
            }
        } else if (cmd == "edit") {
            int id;
            if (!(iss >> id)) {
                std::cout << "Usage: edit <id> [-t title] [-d description] [-p priority] [-D deadline]\n";
                continue;
            }
            std::string rest;
            std::getline(iss, rest);
            std::string title, description, deadline;
            int priority = -1;

            // Simplified parsing: find -t, -d, -p, -D
            size_t pos;
            pos = rest.find("-t ");
            if (pos != std::string::npos) {
                size_t start = pos + 3;
                size_t end = rest.find(" -", start);
                title = Utils::trim(rest.substr(start, end == std::string::npos ? std::string::npos : end - start));
            }
            pos = rest.find(" -d ");
            if (pos != std::string::npos) {
                size_t start = pos + 4;
                size_t end = rest.find(" -", start);
                description = Utils::trim(rest.substr(start, end == std::string::npos ? std::string::npos : end - start));
            }
            pos = rest.find(" -p ");
            if (pos != std::string::npos) {
                size_t start = pos + 4;
                std::istringstream tmp(rest.substr(start));
                tmp >> priority;
            }
            pos = rest.find(" -D ");
            if (pos != std::string::npos) {
                size_t start = pos + 4;
                size_t end = rest.find(" -", start);
                deadline = Utils::trim(rest.substr(start, end == std::string::npos ? std::string::npos : end - start));
            }

            if (mgr.editTask(id, title, description, priority, deadline)) {
                std::cout << "Edited task " << id << '\n';
            } else {
                std::cout << "Task " << id << " not found.\n";
            }
        } else {
            std::cout << "Unknown command. Type 'help' for commands.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
