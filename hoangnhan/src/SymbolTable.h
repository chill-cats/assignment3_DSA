#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

template<typename T>
class FixedSizeVec {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    std::unique_ptr<value_type[]> m_data;
    size_type m_size = 0;

public:
    FixedSizeVec();
    explicit FixedSizeVec(size_type size);
    FixedSizeVec(const FixedSizeVec &other);
    FixedSizeVec(FixedSizeVec &&other) noexcept;
    template<typename Iter>
    FixedSizeVec(Iter begin, Iter end);
    ~FixedSizeVec() = default;

    FixedSizeVec &operator=(const FixedSizeVec &other);
    FixedSizeVec &operator=(FixedSizeVec &&other) noexcept;

    reference operator[](size_type index);
    const_reference operator[](size_type index) const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    size_type size() const noexcept;
    bool empty() const noexcept;
};
template<typename T>
FixedSizeVec<T>::FixedSizeVec() : m_data(std::make_unique<value_type[]>(0)) {}    // NOLINT

template<typename T>
FixedSizeVec<T>::FixedSizeVec(size_type size) : m_data(std::make_unique<value_type[]>(size)), m_size(size) {}    // NOLINT

template<typename T>
FixedSizeVec<T>::FixedSizeVec(const FixedSizeVec &other) : m_data(std::make_unique<value_type[]>(other.m_size)), m_size(other.m_size) {    // NOLINT
    for (size_type i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

template<typename T>
FixedSizeVec<T>::FixedSizeVec(FixedSizeVec &&other) noexcept : m_data(std::move(other.m_data)), m_size(other.m_size) {    // NOLINT
    other.m_size = 0;
}

template<typename T>
template<typename Iter>
FixedSizeVec<T>::FixedSizeVec(Iter begin, Iter end) : m_data(std::make_unique<value_type[]>(end - begin)), m_size(end - begin) {    // NOLINT
    size_type index = 0;
    for (const auto &it = begin; it != end; ++it) {
        m_data[index++] = it;
    }
}

template<typename T>
FixedSizeVec<T> &FixedSizeVec<T>::operator=(const FixedSizeVec &other) {
    if (&other == this) {
        return *this;
    }
    m_data = std::make_unique<value_type[]>(other.m_size);    // NOLINT
    m_size = other.m_size;
    for (size_type i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    return *this;
}

template<typename T>
FixedSizeVec<T> &FixedSizeVec<T>::operator=(FixedSizeVec &&other) noexcept {
    m_data = std::move(other.m_data);
    m_size = other.m_size;
    other.m_size = 0;
    return *this;
}

template<typename T>
typename FixedSizeVec<T>::reference FixedSizeVec<T>::operator[](size_type index) {
    return m_data[index];
}

template<typename T>
typename FixedSizeVec<T>::const_reference FixedSizeVec<T>::operator[](size_type index) const {
    return m_data[index];
}

template<typename T>
typename FixedSizeVec<T>::iterator FixedSizeVec<T>::begin() noexcept {
    return m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::begin() const noexcept {
    return m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::cbegin() const noexcept {
    return m_data.get();
}

template<typename T>
typename FixedSizeVec<T>::iterator FixedSizeVec<T>::end() noexcept {
    return m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::end() const noexcept {
    return m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::const_iterator FixedSizeVec<T>::cend() const noexcept {
    return m_data.get() + m_size;
}

template<typename T>
typename FixedSizeVec<T>::reverse_iterator FixedSizeVec<T>::rbegin() noexcept {
    return std::reverse_iterator<iterator>(m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::rbegin() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::crbegin() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get() + m_size);
}

template<typename T>
typename FixedSizeVec<T>::reverse_iterator FixedSizeVec<T>::rend() noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get());
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::rend() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get());
}

template<typename T>
typename FixedSizeVec<T>::const_reverse_iterator FixedSizeVec<T>::crend() const noexcept {
    return std::reverse_iterator<const_iterator>(m_data.get());
}
template<typename T>
typename FixedSizeVec<T>::size_type FixedSizeVec<T>::size() const noexcept {
    return m_size;
}

template<typename T>
bool FixedSizeVec<T>::empty() const noexcept {
    return size() == 0;
}

class Symbol {
public:
    enum class SymbolType {
        VAR,
        FUNC,
    };

    enum class DataType {
        UN_INFERRED,
        NUMBER,
        STRING,
    };

private:
    const SymbolType symbolType;
    const std::string name;
    const unsigned long level = 0;
    DataType dataType = DataType::UN_INFERRED;    // data type is always uninferred when insert

protected:
    Symbol(std::string name, unsigned long level, SymbolType symbolType);

public:
    const std::string &getName() const noexcept;
    unsigned long getLevel() const noexcept;
    DataType getDataType() const noexcept;
    void setDataType(DataType type) noexcept;
};


class SymbolTable {
    FixedSizeVec<std::unique_ptr<Symbol>> container;

    int currentLevel = 0;
    bool printFlag = false;

    std::string processLine(const std::string &line);
    void detectUnclosedBlock() const;
    void begin() noexcept;
    void end();

    int lookup(const std::string &name, const std::string &line);

public:
    void run(const string &filename);
};
#endif
