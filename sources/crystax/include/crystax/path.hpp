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

#ifndef _CRYSTAX_PATH_HPP_e07f4133db9547fc910f4c7e2f6a8a35
#define _CRYSTAX_PATH_HPP_e07f4133db9547fc910f4c7e2f6a8a35

#include <crystax/common.hpp>
#include <crystax/memory.hpp>
#include <crystax/jutils.hpp>

namespace crystax
{
namespace fileio
{

namespace details
{

class path_t
{
    typedef void (path_t::*safe_bool_type)() const;
    void non_empty_path() const {}
public:
    explicit path_t(const char *path = NULL)
        :p(0), len(0)
    {
        reset(path);
    }

    bool operator!() const {return !p;}
    operator safe_bool_type () const
    {
        return !p ? 0 : &path_t::non_empty_path;
    }

    const char &operator*() const {return *p;}

    const char &operator[](size_t index) const {return p.get()[index];}

    bool empty() const {return *p == '\0';}

    virtual void reset(const char *path = NULL)
    {
        p.reset(path);
        len = path ? ::strlen(path) : 0;
    }

    const char *release()
    {
        len = 0;
        return p.release();
    }

    const char *c_str() const {return p.get();}
    size_t length() const {return len;}

    bool subpath(path_t const &root) const {return subpath(root.c_str());}
    bool subpath(const char *root) const {return is_subpath(root, c_str());}

    const char *relpath(path_t const &root) const {return ::crystax::fileio::relpath(root.c_str(), c_str());}
    const char *relpath(const char *root) const {return ::crystax::fileio::relpath(root, c_str());}

    const char *basename() const {return ::crystax::fileio::basename(c_str());}
    const char *dirname() const {return ::crystax::fileio::dirname(c_str());}

    path_t &operator+=(path_t const &path) {return *this += path.c_str();}
    path_t &operator+=(const char *path)
    {
        if (!path || *path == '\0')
            return *this;

        char *ns = (char *)::malloc(length() + ::strlen(path) + 2);
        ::strcpy(ns, p.get());
        ::strcat(ns, "/");
        ::strcat(ns, path);

        if (is_normalized(ns))
            reset(ns);
        else
        {
            reset(normalize(ns));
            ::free((void*)ns);
        }

        return *this;
    }

private:
    bool n;
    scope_c_ptr_t<const char> p;
    size_t len;
};

inline
bool operator==(path_t const &a, path_t const &b)
{
    if (!a || !b)
        return a.c_str() == b.c_str();
    return a.length() == b.length() && ::strcmp(a.c_str(), b.c_str()) == 0;
}

inline
bool operator!=(path_t const &a, path_t const &b)
{
    return !(a == b);
}

} // namespace details

class path_t : public details::path_t
{
public:
    explicit path_t(const char *path = NULL)
        :details::path_t(normalize(path))
    {}

    void reset(const char *path = NULL)
    {
        details::path_t::reset(normalize(path));
    }
};

template <typename T>
T jcast(path_t const &p)
{
    return ::crystax::jni::details::jcast_helper<T, const char *>::cast(p.c_str());
}

class abspath_t : public details::path_t
{
public:
    explicit abspath_t(const char *path = NULL)
        :details::path_t(absolutize(path))
    {}

    void reset(const char *path = NULL)
    {
        details::path_t::reset(absolutize(path));
    }
};

template <typename T>
T jcast(abspath_t const &p)
{
    return ::crystax::jni::details::jcast_helper<T, const char *>::cast(p.c_str());
}

} // namespace fileio
} // namespace crystax

#endif // _CRYSTAX_PATH_HPP_e07f4133db9547fc910f4c7e2f6a8a35
