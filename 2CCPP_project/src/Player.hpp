#pragma once
#include <string>

class Player
{
public:
    Player(int id, std::string name) : m_id(id), m_name(std::move(name)) {}

    int id() const { return m_id; }
    const std::string &name() const { return m_name; }

    int getScore() const { return m_score; }
    void setScore(int s) { m_score = s; }

private:
    int m_id = -1;
    std::string m_name;
    int m_score = 0;
};
