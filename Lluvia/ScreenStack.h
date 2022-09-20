#pragma once

#include "App.h"

namespace t
{
    enum class Stack_actions
    {
        None, Push, Pop, Clear
    };

    enum class Screens
    {
        None,
        Initial, Loading,
        Chart_Select, Stagefile_Present, Play, Pause,
        Result_Normal, Result_Course,
        Options, Injections, Skin_Change, Key_Binding,
        Notice
    };

    struct Pending_change
    {
        t::Stack_actions action = t::Stack_actions::None;
        t::Screens screen = t::Screens::None;
    };

    class Screen;

    class Screen_stack {
    public:
        Screen_stack(t::Context* context);

        void register_screen(t::Screens screen);

        void handle_event();
        void update();
        void draw();

        void add_push_request(t::Screens screen);
        void add_pop_request();
        void add_clear_request();

        bool empty() const;
    private:
        //std::unique_ptr<State> create_state(Id_states iid_state);
        void apply_pending_changes();

        t::Context* m_context = nullptr;
        std::vector<std::unique_ptr<t::Screen>> m_stack;
        std::vector<t::Pending_change> m_pending_list;
        //std::map<Id_states, std::function<std::unique_ptr<State>()>> factories;
    };

    class Screen {
    public:
        Screen(t::Screen_stack* stack, t::Context* context, t::Engine* engine) noexcept;
        virtual ~Screen() = default;

        virtual void handle_event() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;
    protected:
        void request_stack_push(t::Screens screen);
        void request_stack_pop();
        void request_stack_clear();
    private:
        t::Context* m_context = nullptr;
        t::Screen_stack* m_stack = nullptr;
        t::Engine* m_engine = nullptr;
    };
}