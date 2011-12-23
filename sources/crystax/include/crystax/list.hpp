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

#ifndef _CRYSTAX_LIST_HPP_fac4d8667aa24cb0a843360a1d7d8cad
#define _CRYSTAX_LIST_HPP_fac4d8667aa24cb0a843360a1d7d8cad

#include <crystax/common.hpp>
#include <stddef.h>

namespace crystax
{

template <typename T>
class list_t : public non_copyable_t
{
public:
    list_t() :phead(0), ptail(0), sz(0) {}
    ~list_t() {clear();}

    T *head() const {return phead;}
    T *tail() const {return ptail;}

    bool empty() const {return sz == 0;}

    size_t size() const {return sz;}

    void clear()
    {
        while (phead)
        {
            T *node = phead;
            phead = phead->next;
            delete node;
        }
        phead = ptail = 0;
        sz = 0;
    }

    bool push(T *before, T *node)
    {
        if (!before || !node) return false;

        T *after = before->next;
        before->next = node;
        if (after) after->prev = node;
        node->prev = before;
        node->next = after;
        ++sz;
        return true;
    }

    T *pop(T *node)
    {
        if (!node) return 0;
        if (node->prev) node->prev->next = node->next;
        if (node->next) node->next->prev = node->prev;
        if (node == phead)
            phead = phead->next;
        else if (node == ptail)
            ptail = ptail->prev;
        if (phead == 0)
            ptail = 0;
        if (ptail == 0)
            phead = 0;
        node->next = node->prev = 0;
        --sz;
        return node;
    }

    bool push_front(T *node)
    {
        if (!phead)
        {
            phead = ptail = node;
            sz = 1;
            return true;
        }
        return push(phead, node);
    }

    bool push_back(T *node)
    {
        if (!phead)
        {
            phead = ptail = node;
            sz = 1;
            return true;
        }
        if (!push(ptail, node))
            return false;
        ptail = node;
        return true;
    }

    T *pop_front()
    {
        return pop(phead);
    }

    T *pop_back()
    {
        return pop(ptail);
    }

    template <typename Comparator, typename V>
    T *find(Comparator c, V const &v)
    {
        for (T *node = phead; node; node = node->next)
        {
            if (c(*node, v))
                return node;
        }

        return 0;
    }

private:
    T *phead;
    T *ptail;
    size_t sz;
};

} // namespace crystax

#endif // _CRYSTAX_LIST_HPP_fac4d8667aa24cb0a843360a1d7d8cad
