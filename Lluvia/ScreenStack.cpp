#include "pch.h"
#include "ScreenStack.h"

t::Screen_stack::Screen_stack(t::Context* context)
    : m_context(context)
{
    m_stack.reserve(4);
    m_pending_list.reserve(6);
}

void t::Screen_stack::add_push_request(t::Screens screen)
{
    m_pending_list.emplace_back(t::Stack_actions::Push, screen);
}

void t::Screen_stack::add_pop_request()
{
    m_pending_list.emplace_back(t::Stack_actions::Pop, t::Screens::None);
}

void t::Screen_stack::add_clear_request()
{
    m_pending_list.emplace_back(t::Stack_actions::Clear, t::Screens::None);
}

bool t::Screen_stack::empty() const
{
    return m_stack.empty();
}

/*
       lllllllllllllllllllllll
      lllllllllllllllllllllllll
     llllllllll       llllllllll
    llllllllll         llllllllll
   lllllllllllllllllllllllllllllll
  lllllllllllllllllllllllllllllllll
 llllllllll               llllllllll
llllllllll                 llllllllll
*/

t::Screen::Screen(t::Screen_stack* stack, t::Context* context, t::Engine* engine) noexcept
    : m_stack(stack),
    m_context(context),
    m_engine(engine)
{
}

void t::Screen::request_stack_push(t::Screens screen)
{
    m_stack->add_push_request(screen);
}

void t::Screen::request_stack_pop()
{
    m_stack->add_pop_request();
}

void t::Screen::request_stack_clear()
{
    m_stack->add_clear_request();
}
