#pragma once

#include <type_traits>
#include <utility>

template <class _Tp>
struct DefaultDeleter { // Ĭ��ʹ�� delete �ͷ��ڴ�
    void operator()(_Tp* p) const {
        delete p;
    }
};

template <class _Tp>
struct DefaultDeleter<_Tp[]> { // ƫ�ػ�
    void operator()(_Tp* p) const {
        delete[] p;
    }
};

template <class _Tp, class _Deleter = DefaultDeleter<_Tp>>
struct UniquePtr {
private:
    _Tp* _M_p;
    [[no_unique_address]] _Deleter _M_deleter;

    template <class _Up, class _UDeleter>
    friend struct UniquePtr;

public:
    using element_type = _Tp;
    using pointer = _Tp*;
    using deleter_type = _Tp;

    UniquePtr(std::nullptr_t = nullptr) noexcept : _M_p(nullptr) { // Ĭ�Ϲ��캯��
    }

    explicit UniquePtr(_Tp* p) noexcept : _M_p(p) { // �Զ��幹�캯��
    }

    template <class _Up, class _UDeleter, class = std::enable_if_t<std::is_convertible_v<_Up*, _Tp*>>> // û�� C++20 ��д��
    // template <class _Up, class _UDeleter> requires (std::convertible_to<_Up *, _Tp *>) // �� C++20 ��д��
    UniquePtr(UniquePtr<_Up, _UDeleter>&& __that) noexcept : _M_p(__that._M_p) {  // ��������_Up������ָ��ת����_Tp���͵�����ָ��
        __that._M_p = nullptr;
    }

    ~UniquePtr() noexcept { // ��������
        if (_M_p)
            _M_deleter(_M_p);
    }

    UniquePtr(UniquePtr const& __that) = delete; // �������캯��
    UniquePtr& operator=(UniquePtr const& __that) = delete; // ������ֵ����

    UniquePtr(UniquePtr&& __that) noexcept : _M_p(__that._M_p) { // �ƶ����캯��
        __that._M_p = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& __that) noexcept { // �ƶ���ֵ����
        if (this != &__that) [[likely]] {
            if (_M_p)
                _M_deleter(_M_p);
            _M_p = exchange(__that._M_p, nullptr);
        }
        return *this;
    }

    void swap(UniquePtr& __that) noexcept { // ��������
        std::swap(_M_p, __that._M_p);
    }

    _Tp* get() const noexcept {
        return _M_p;
    }

    _Tp* operator->() const noexcept {
        return _M_p;
    }

    std::add_lvalue_reference_t<_Tp> operator*() const noexcept {
        return *_M_p;
    }

    _Deleter get_deleter() const noexcept {
        return _M_deleter;
    }

    _Tp* release() noexcept {
        _Tp* __p = _M_p;
        _M_p = nullptr;
        return __p;
    }

    void reset(_Tp* __p = nullptr) noexcept {
        if (_M_p)
            _M_deleter(_M_p);
        _M_p = __p;
    }

    explicit operator bool() const noexcept {
        return _M_p != nullptr;
    }

    bool operator==(UniquePtr const& __that) const noexcept {
        return _M_p == __that._M_p;
    }

    bool operator!=(UniquePtr const& __that) const noexcept {
        return _M_p != __that._M_p;
    }

    bool operator<(UniquePtr const& __that) const noexcept {
        return _M_p < __that._M_p;
    }

    bool operator<=(UniquePtr const& __that) const noexcept {
        return _M_p <= __that._M_p;
    }

    bool operator>(UniquePtr const& __that) const noexcept {
        return _M_p > __that._M_p;
    }

    bool operator>=(UniquePtr const& __that) const noexcept {
        return _M_p >= __that._M_p;
    }
};

template <class _Tp, class _Deleter>
struct UniquePtr<_Tp[], _Deleter> : UniquePtr<_Tp, _Deleter> {
    using UniquePtr<_Tp, _Deleter>::UniquePtr;

    std::add_lvalue_reference_t<_Tp> operator[](std::size_t __i) {
        return this->get()[__i];
    }
};

template <class _Tp, class ..._Args, std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> = 0>
UniquePtr<_Tp> makeUnique(_Args &&...__args) {
    return UniquePtr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

template <class _Tp, std::enable_if_t<!std::is_unbounded_array_v<_Tp>, int> = 0>
UniquePtr<_Tp> makeUniqueForOverwrite() {
    return UniquePtr<_Tp>(new _Tp);
}

template <class _Tp, class ..._Args, std::enable_if_t<std::is_unbounded_array_v<_Tp>, int> = 0>
UniquePtr<_Tp> makeUnique(std::size_t __len) {
    return UniquePtr<_Tp>(new std::remove_extent_t<_Tp>[__len]());
}

template <class _Tp, std::enable_if_t<std::is_unbounded_array_v<_Tp>, int> = 0>
UniquePtr<_Tp> makeUniqueForOverwrite(std::size_t __len) {
    return UniquePtr<_Tp>(new std::remove_extent_t<_Tp>[__len]);
}
