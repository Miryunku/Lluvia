#pragma once

namespace t
{
    template<typename T>
    class Mallocker {
    public:
        Mallocker(size_t size);

        Mallocker(const Mallocker& other) = delete;
        Mallocker& operator= (const Mallocker& other) = delete;

        Mallocker(Mallocker&& other) noexcept;
        Mallocker& operator= (Mallocker&& other) noexcept;

        ~Mallocker() noexcept;

        T& operator[](size_t index);
        const T& operator[](size_t index) const;

        T* data() noexcept;
        size_t size() const noexcept;
    private:
        T* m_buffer = nullptr;
        size_t m_size = 0;
    };
}