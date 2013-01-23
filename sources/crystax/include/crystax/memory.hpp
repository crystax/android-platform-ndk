/*
 * Copyright (c) 2011-2012 Dmitry Moskalchuk <dm@crystax.net>.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY Dmitry Moskalchuk ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Dmitry Moskalchuk OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Dmitry Moskalchuk.
 */

#ifndef _CRYSTAX_MEMORY_HPP_082889d25d1243759f873416990f0023
#define _CRYSTAX_MEMORY_HPP_082889d25d1243759f873416990f0023

#include <crystax/common.hpp>

namespace crystax
{

namespace details
{

struct free_policy_t
{
    template <typename T>
    static void free(T *ptr)
    {
        ::free((void*)ptr);
    }
};

struct delete_ptr_policy_t
{
    template <typename T>
    static void free(T *ptr)
    {
        delete ptr;
    }
};

struct delete_array_policy_t
{
    template <typename T>
    static void free(T *ptr)
    {
        delete [] ptr;
    }
};

template <typename T, typename DeletePolicy>
class scope_ptr_t : public non_copyable_t
{
public:
    explicit scope_ptr_t(T *p)
        :ptr(p)
    {}

    ~scope_ptr_t() {reset();}

    bool operator!() const {return !ptr;}
    T *operator->() const {return ptr;}
    T *get() const {return ptr;}
    T &operator*() const {return *ptr;}

    T const &operator[](size_t index) const {return ptr[index];}
    T &operator[](size_t index) {return ptr[index];}

    void reset(T *p = 0)
    {
        DeletePolicy::free(ptr);
        ptr = p;
    }

    T *release()
    {
        T *p = ptr;
        ptr = 0;
        return p;
    }

    void swap(scope_ptr_t &c)
    {
        T *tmp = ptr;
        ptr = c.ptr;
        c.ptr = tmp;
    }

private:
    T *ptr;
};

template <typename T, typename DeletePolicy>
void swap(scope_ptr_t<T, DeletePolicy> &a, scope_ptr_t<T, DeletePolicy> &b)
{
    a.swap(b);
}

} // namespace details

template <typename T>
class scope_c_ptr_t : public details::scope_ptr_t<T, details::free_policy_t>
{
public:
    explicit scope_c_ptr_t(T *p)
        :details::scope_ptr_t<T, details::free_policy_t>(p)
    {}
};

template <typename T>
class scope_cpp_ptr_t : public details::scope_ptr_t<T, details::delete_ptr_policy_t>
{
public:
    explicit scope_cpp_ptr_t(T *p)
        :details::scope_ptr_t<T, details::delete_ptr_policy_t>(p)
    {}
};

template <typename T>
class scope_cpp_array_t : public details::scope_ptr_t<T, details::delete_array_policy_t>
{
public:
    explicit scope_cpp_array_t(T *p)
        :details::scope_ptr_t<T, details::delete_array_policy_t>(p)
    {}
};

} // namespace crystax

#endif /* _CRYSTAX_MEMORY_HPP_082889d25d1243759f873416990f0023 */
