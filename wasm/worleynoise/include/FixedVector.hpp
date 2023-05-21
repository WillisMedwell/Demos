/*
    Purpose: An array with vector like properties
    Created by: Willis Medwell
    Date: 14/Nov/2022
*/
#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <span>
#include <stdexcept>

template <typename T, std::size_t S>
class FixedVector {
public:
    // types
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = ptrdiff_t;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    value_type m_data[S];
    size_type m_capacity;
    size_type m_size;

public:
    // constructors
    constexpr FixedVector()
        : m_capacity(S)
        , m_size(0)
    {
    }
    constexpr FixedVector(std::initializer_list<T> list)
        : m_capacity(S)
        , m_size(list.size())
    {
        std::span<T> tmp = { m_data, S };
        std::copy(list.begin(), list.end(), tmp.begin());
    }
    // iterators
    [[nodiscard]] constexpr iterator begin() noexcept;
    [[nodiscard]] constexpr const_iterator begin() const noexcept;
    [[nodiscard]] constexpr iterator end() noexcept;
    [[nodiscard]] constexpr const_iterator end() const noexcept;

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept;
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] constexpr reverse_iterator rend() noexcept;
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept;
    [[nodiscard]] constexpr const_iterator cend() const noexcept;
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;

    // capacity
    [[nodiscard]] constexpr bool empty() const noexcept;
    [[nodiscard]] constexpr size_type size() const noexcept;
    [[nodiscard]] constexpr size_type capacity() const noexcept;

    // element access
    [[nodiscard]] constexpr reference operator[](std::size_t n);
    [[nodiscard]] constexpr const_reference operator[](std::size_t n) const;
    [[nodiscard]] constexpr reference at(std::size_t n);
    [[nodiscard]] constexpr const_reference at(std::size_t n) const;
    [[nodiscard]] constexpr reference front();
    [[nodiscard]] constexpr const_reference front() const;
    [[nodiscard]] constexpr reference back();
    [[nodiscard]] constexpr const_reference back() const;
    // data access
    [[nodiscard]] constexpr T* data() noexcept;
    [[nodiscard]] constexpr const T* data() const noexcept;
    // modifiers
    template <class... Args>
    constexpr reference emplace_back(Args&&... args);
    template <class... Args>
    constexpr reference emplace_front(Args&&... args);
    constexpr void push_back(const T& x);
    constexpr void push_back(T&& x);
    constexpr void push_front(const T& x);
    constexpr void push_front(T&& x);
    constexpr void pop_back();
    constexpr void pop_front();

    template <class... Args>
    constexpr iterator emplace(const_iterator position, Args&&... args);
    constexpr iterator insert(const_iterator position, const T& x);
    constexpr iterator insert(const_iterator position, T&& x);

    // TODO maybe...
    // constexpr iterator insert(const_iterator position, size_type n, const T&
    // x); template<class InputIt> constexpr iterator insert(const_iterator
    // position, InputIt first, InputIt last); constexpr iterator
    // insert(const_iterator position, std::initializer_list<T> il);
    constexpr iterator erase(const_iterator position);
    constexpr iterator erase(iterator first, iterator last);
    constexpr void clear() noexcept;
    constexpr void resize(std::size_t n);
    constexpr void resize(iterator first, iterator last);
};

template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::iterator
FixedVector<T, S>::begin() noexcept
{
    return &m_data[0];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_iterator
FixedVector<T, S>::begin() const noexcept
{
    return &m_data[0];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::iterator
FixedVector<T, S>::end() noexcept
{
    return &m_data[m_size];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_iterator
FixedVector<T, S>::end() const noexcept
{
    return &m_data[m_size];
}

template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::reverse_iterator
FixedVector<T, S>::rbegin() noexcept
{
    return std::reverse_iterator(this->end());
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reverse_iterator
FixedVector<T, S>::rbegin() const noexcept
{
    return std::reverse_iterator(this->end());
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::reverse_iterator
FixedVector<T, S>::rend() noexcept
{
    return std::reverse_iterator(this->begin());
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reverse_iterator
FixedVector<T, S>::rend() const noexcept
{
    return std::reverse_iterator(this->begin());
}

template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_iterator
FixedVector<T, S>::cbegin() const noexcept
{
    return &m_data[0];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_iterator
FixedVector<T, S>::cend() const noexcept
{
    return &m_data[m_size];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reverse_iterator
FixedVector<T, S>::crbegin() const noexcept
{
    return std::reverse_iterator(this->end());
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reverse_iterator
FixedVector<T, S>::crend() const noexcept
{
    return std::reverse_iterator(this->begin());
}

template <typename T, std::size_t S>
[[nodiscard]] constexpr bool FixedVector<T, S>::empty() const noexcept
{
    return (m_size == 0);
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr std::size_t FixedVector<T, S>::size() const noexcept
{
    return m_size;
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr std::size_t FixedVector<T, S>::capacity()
    const noexcept
{
    return m_capacity;
}

template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::reference
FixedVector<T, S>::operator[](std::size_t n)
{
    if (n < 0 or n >= m_size) [[unlikely]] {
        // throw std::out_of_range("ERROR: Trying to access out of range element");
    }
    return m_data[n];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reference
FixedVector<T, S>::operator[](std::size_t n) const
{
    if (n < 0 or n >= m_size) [[unlikely]] {
        // throw std::out_of_range("ERROR: Trying to access out of range element");
    }
    return m_data[n];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::reference
FixedVector<T, S>::at(std::size_t n)
{
    if (n < 0 or n >= m_size) [[unlikely]] {
        // throw std::out_of_range("ERROR: Trying to access out of range element");
    }
    return m_data[n];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reference
FixedVector<T, S>::at(std::size_t n) const
{
    if (n < 0 or n >= m_size) [[unlikely]] {
        // throw std::out_of_range("ERROR: Trying to access out of range element");
    }
    return m_data[n];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::reference
FixedVector<T, S>::front()
{
    if (m_size == 0) [[unlikely]] {
        // throw std::out_of_range("ERROR: no element at front");
    }
    return m_data[0];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reference
FixedVector<T, S>::front() const
{
    if (m_size == 0) [[unlikely]] {
        // throw std::out_of_range("ERROR: no element at front");
    }
    return m_data[0];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::reference
FixedVector<T, S>::back()
{
    if (m_size == 0) [[unlikely]] {
        // throw std::out_of_range("ERROR: no element at front");
    }
    return m_data[m_size - 1];
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr typename FixedVector<T, S>::const_reference
FixedVector<T, S>::back() const
{
    if (m_size == 0) [[unlikely]] {
        // throw std::out_of_range("ERROR: no element at front");
    }
    return m_data[m_size - 1];
}

template <typename T, std::size_t S>
[[nodiscard]] constexpr T* FixedVector<T, S>::data() noexcept
{
    return m_data;
}
template <typename T, std::size_t S>
[[nodiscard]] constexpr const T* FixedVector<T, S>::data() const noexcept
{
    return m_data;
}

template <typename T, std::size_t S>
template <class... Args>
constexpr typename FixedVector<T, S>::reference FixedVector<T, S>::emplace_back(
    Args&&... args)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    m_data[m_size] = (T(std::forward<Args>(args)...));
    ++m_size;
    return m_data[m_size - 1];
}
template <typename T, std::size_t S>
template <class... Args>
constexpr typename FixedVector<T, S>::reference
FixedVector<T, S>::emplace_front(Args&&... args)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    // std::memcpy(&m_data[1], &m_data[0], m_size*sizeof(T));
    std::copy(begin(), end(), begin() + 1);
    m_data[0] = (T(std::forward<Args>(args)...));
    ++m_size;

    return m_data[0];
}

template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::push_back(const T& x)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    m_data[m_size] = x;
    ++m_size;
}
template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::push_back(T&& x)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    m_data[m_size] = std::move(x);
    ++m_size;
}

template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::push_front(const T& x)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    // std::memcpy(&m_data[1], &m_data[0], m_size*sizeof(T));
    std::copy(begin(), end(), begin() + 1);
    m_data[0] = x;
    ++m_size;
}
template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::push_front(T&& x)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    // std::memcpy(&m_data[1], &m_data[0], m_size*sizeof(T));
    std::copy(begin(), end(), begin() + 1);
    m_data[0] = std::move(x);
    ++m_size;
}

template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::pop_back()
{
    if (m_size == 0) [[unlikely]] {
        // throw std::length_error("ERROR: nothing to pop_back");
    }
    --m_size;
}
template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::pop_front()
{
    if (m_size == 0) [[unlikely]] {
        // throw std::length_error("ERROR: nothing to pop_back");
    }
    // std::memcpy(&m_data[0], &m_data[1], (m_size-1)*sizeof(T));
    std::copy(begin() + 1, end(), begin());
    --m_size;
}

template <typename T, std::size_t S>
template <class... Args>
constexpr typename FixedVector<T, S>::iterator FixedVector<T, S>::emplace(
    const_iterator position,
    Args&&... args)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    const auto index = static_cast<std::size_t>(position - m_data);
    if (index < 0 or index > (m_size)) [[unlikely]] {
        // throw std::out_of_range("ERROR: iterator not in valid range");
    }
    // std::memcpy(&m_data[index+1], &m_data[index], (m_size-index)*sizeof(T));
    std::copy(begin() + index, end(), begin() + index + 1);
    m_data[index] = (T(std::forward<Args>(args)...));
    m_size++;
    return &m_data[index];
}
template <typename T, std::size_t S>
constexpr typename FixedVector<T, S>::iterator FixedVector<T, S>::insert(
    const_iterator position,
    const T& x)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    const auto index = static_cast<std::size_t>(position - m_data);
    if (index < 0 or index > (m_size)) [[unlikely]] {
        // throw std::out_of_range("ERROR: iterator not in valid range");
    }
    std::copy(begin() + index, end(), begin() + index + 1);
    m_data[index] = x;
    m_size++;
    return &m_data[index];
}
template <typename T, std::size_t S>
constexpr typename FixedVector<T, S>::iterator FixedVector<T, S>::insert(
    const_iterator position,
    T&& x)
{
    if (m_size == m_capacity) [[unlikely]] {
        // throw std::length_error("ERROR: exceeded maximum capacity");
    }
    const auto index = static_cast<std::size_t>(position - m_data);
    if (index < 0 or index > (m_size)) [[unlikely]] {
        // throw std::out_of_range("ERROR: iterator not in valid range");
    }
    std::copy(begin() + index, end(), begin() + index + 1);
    m_data[index] = std::move(x);
    m_size++;
    return &m_data[index];
}
// TODO maybe...
// constexpr iterator insert(const_iterator position, size_type n, const T& x);
// template<class InputIt> constexpr iterator insert(const_iterator position,
// InputIt first, InputIt last); constexpr iterator insert(const_iterator
// position, std::initializer_list<T> il);

template <typename T, std::size_t S>
constexpr typename FixedVector<T, S>::iterator FixedVector<T, S>::erase(
    const_iterator position)
{
    const auto index = static_cast<std::size_t>(position - m_data);
    if (index < 0 or index >= (m_size)) [[unlikely]] {
        // throw std::out_of_range("ERROR: iterator not in valid range");
    }
    std::copy(begin() + index + 1, end(), begin() + index);
    m_size--;
    return &m_data[index];
}

template <typename T, std::size_t S>
constexpr typename FixedVector<T, S>::iterator FixedVector<T, S>::erase(
    iterator first,
    iterator last)
{
    const auto n = size() - static_cast<std::size_t>(std::distance(first, last));
    if (first > last) {
        // throw std::runtime_error("ERROR: begin iterator cannot be greater than
        // end iterator");
    }
    if (first < begin() || first > end()) {
        // throw std::runtime_error("ERROR: bad begin iterator");
    }
    if (last > end() || last < begin()) {
        // throw std::runtime_error("ERROR: bad end iterator");
    }
    auto second_begin = begin() + std::distance(begin(), first);
    std::copy(last, end(), second_begin);
    m_size = n;
    return end();
}

template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::clear() noexcept
{
    m_size = 0;
}
template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::resize(std::size_t n)
{
    if (n > m_capacity) {
        // throw std::length_error("ERROR: fixed vector cannot resize to that
        // length");
    }
    m_size = n;
}

template <typename T, std::size_t S>
constexpr void FixedVector<T, S>::resize(FixedVector<T, S>::iterator first,
    FixedVector<T, S>::iterator last)
{
    const auto n = static_cast<std::size_t>(std::distance(first, last));
    if (first > last) {
        // throw std::runtime_error("ERROR: begin iterator cannot be greater than
        // end iterator");
    }
    if (first < begin() || first > end()) {
        // throw std::runtime_error("ERROR: bad begin iterator");
    }
    if (last > end() || last < begin()) {
        // throw std::runtime_error("ERROR: bad end iterator");
    }
    std::copy(first, last, m_data);
    m_size = n;
}