#pragma once

#include <functional>
#include <vector>

template<typename... Args>
class Event
{
public:
    using Listener = std::function<void(Args...)>;

    void Subscribe(const Listener& listener) { m_Listeners.push_back(listener); }

    void Unsubscribe(const Listener& listener)
    {
        auto it = std::find_if(
            m_Listeners.begin(), m_Listeners.end(), [&](const Listener& l) { return IsSameFunction(l, listener); });
        if (it != m_Listeners.end())
        {
            m_Listeners.erase(it);
        }
    }

    void Invoke(Args... args)
    {
        for (const auto& listener : m_Listeners)
        {
            listener(args...);
        }
    }

private:
    // Helper function to check if two std::function objects are the same
    bool IsSameFunction(const Listener& func1, const Listener& func2) const
    {
        return func1.target_type() == func2.target_type() && func1.target<void()>() == func2.target<void()>();
    }

private:
    std::vector<Listener> m_Listeners;
};

// Specialization for void events
template<>
class Event<void>
{
public:
    using Listener = std::function<void()>;

    void Subscribe(const Listener& listener) { m_Listeners.push_back(listener); }

    void Unsubscribe(const Listener& listener)
    {
        auto it = std::find_if(
            m_Listeners.begin(), m_Listeners.end(), [&](const Listener& l) { return IsSameFunction(l, listener); });
        if (it != m_Listeners.end())
        {
            m_Listeners.erase(it);
        }
    }

    void Invoke()
    {
        for (const auto& listener : m_Listeners)
        {
            listener();
        }
    }

private:
    // Helper function to check if two std::function objects are the same
    bool IsSameFunction(const Listener& func1, const Listener& func2) const
    {
        return func1.target_type() == func2.target_type() && func1.target<void()>() == func2.target<void()>();
    }

private:
    std::vector<Listener> m_Listeners;
};