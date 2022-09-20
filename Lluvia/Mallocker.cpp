#include "pch.h"
#include "Mallocker.h"

template<typename T>
t::Mallocker<T>::Mallocker(size_t size)
    : m_size(size)
{
    m_buffer = reinterpret_cast<T*>(malloc(size));
    if (!m_buffer)
    {
        throw std::bad_alloc();
    }
}

template<typename T>
t::Mallocker<T>::Mallocker(Mallocker&& other) noexcept
    : m_buffer(other.m_buffer),
    m_size(other.m_size)
{
    other.m_buffer = nullptr;
    other.m_size = 0;
}

template<typename T>
t::Mallocker<T>& t::Mallocker<T>::operator=(Mallocker&& other) noexcept
{
    if (this != &other)
    {
        // Free existing resources
        free(m_buffer);

        // Steal
        m_buffer = other.m_buffer;
        m_size = other.m_size;

        // Leave source object with default values
        other.m_buffer = nullptr;
        other.m_size = 0;
    }
    return *this;
}

template<typename T>
t::Mallocker<T>::~Mallocker() noexcept
{
    free(m_buffer);
}

template<typename T>
T& t::Mallocker<T>::operator[](size_t index)
{
    return m_buffer[index];
}

template<typename T>
const T& t::Mallocker<T>::operator[](size_t index) const
{
    return m_buffer[index];
}

template<typename T>
T* t::Mallocker<T>::data() noexcept
{
    return m_buffer;
}

template<typename T>
size_t t::Mallocker<T>::size() const noexcept
{
    return m_size;
}

template t::Mallocker<uint8_t>;
