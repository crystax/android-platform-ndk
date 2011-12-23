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

#if defined(CRYSTAX_DEBUG_PATH_FUNCTIONS) && CRYSTAX_DEBUG_PATH_FUNCTIONS == 1
#   ifdef CRYSTAX_DEBUG
#   undef CRYSTAX_DEBUG
#   endif
#   define CRYSTAX_DEBUG 1
#   define CRYSTAX_DEBUG_SINK 0
#else
#   ifdef CRYSTAX_DEBUG
#   undef CRYSTAX_DEBUG
#   endif
#   define CRYSTAX_DEBUG 0
#   ifdef CRYSTAX_FILEIO_DEBUG
#   undef CRYSTAX_FILEIO_DEBUG
#   endif
#   define CRYSTAX_FILEIO_DEBUG 0
#endif

#include "fileio/common.hpp"

namespace crystax
{
namespace fileio
{

char *normalize(const char *path)
{
    char part[NAME_MAX + 1];
    char buf[PATH_MAX + 1];
    size_t buflen;

    DBG("** path=%s", path);

    if (path == NULL)
    {
        ERR("empty path");
        return NULL;
    }

    buf[0] = '\0';
    buflen = 0;
    for (const char *s = path, *p = path; p; s = p + 1)
    {
        DBG("s=%s", s);
        p = ::strchr(s, '/');
        DBG("p=%s", p);
        size_t partlen = p ? p - s : ::strlen(s);
        if (partlen >= sizeof(part))
            partlen = sizeof(part) - 1;
        ::strncpy(part, s, partlen);
        part[partlen] = '\0';
        DBG("- part=%s", part);

        if (partlen == 0)
        {
            if (buflen == 0 && *path == '/')
            {
                DBG("leading slash found");
                ::strcat(buf, "/");
                ++buflen;
            }
            else
                DBG("ignore empty part");
        }
        else if (partlen == 1 && part[0] == '.')
        {
            if (buflen == 0)
            {
                DBG("'.' part at begin");
                ::strcat(buf, ".");
                ++buflen;
            }
            else
                DBG("ignore '.' part");
        }
        else if (partlen == 2 && part[0] == '.' && part[1] == '.')
        {
            if (buflen == 0)
            {
                DBG("'..' part at begin");
                ::strcat(buf, "..");
                buflen += 2;
            }
            else
            {
                DBG("remove last component");
                char *v = ::strrchr(buf, '/');
                DBG("v=%s", v);
                if (v == NULL)
                {
                    // No '/' found. It's possible only if buf contains single non-empty path component
                    DBG("no '/' found");
                    if (buflen == 1 && buf[0] == '.')
                    {
                        // Special case: '.'
                        ::strcpy(buf, "..");
                        buflen += 2;
                    }
                    else if (buflen == 2 && buf[0] == '.' && buf[1] == '.')
                    {
                        // Special case: '..'
                        ::strcat(buf, "/..");
                        buflen += 3;
                    }
                    else
                    {
                        ::strcpy(buf, ".");
                        buflen = 1;
                    }
                }
                else
                {
                    // Cut last path component
                    DBG("cut last path component");
                    if (v == buf && *buf == '/')
                    {
                        // Special case: '/..'
                        ++v;
                    }

                    DBG("buf=%s, buflen=%u, v=%s", buf, (unsigned)buflen, v);
                    if (buf + buflen - v == 3 && v[0] == '/' && v[1] == '.' && v[2] == '.')
                    {
                        // Special case: '../..'
                        ::strcat(buf, "/..");
                        buflen += 3;
                    }
                    else
                    {
                        buflen = v - buf;
                        *v = '\0';
                    }
                }
            }
        }
        else
        {
            if (buflen == 1 && *buf == '.')
            {
                // We found leading '.' path component. Remove it because we'll add non-empty component now.
                DBG("leading '.' found");
                buflen = 0;
                *buf = '\0';
            }
            if (buflen > 0 && buf[buflen - 1] != '/')
            {
                DBG("add '/' to the end");
                ::strcat(buf, "/");
                ++buflen;
            }
            ::strcat(buf, part);
            buflen += partlen;
        }
        DBG("-- buf=%s", buf);
    }

    DBG("++ result=%s", buf);
    return ::strdup(buf);
}

bool is_normalized(const char *path)
{
    DBG("path=%s", path);

    if (path == NULL || *path == '\0' ||
        (path[0] == '/' && path[1] == '\0'))
    {
        DBG("path is NULL or empty or '/'");
        return true;
    }

    const char *s = path;
    if (*s == '/')
        ++s;
    for (const char *p = s; p; s = p + 1)
    {
        p = strchr(s, '/');
        DBG("p=%s", p);
        size_t partlen = p ? p - s : strlen(s);
        DBG("partlen=%lu", (unsigned long)partlen);
        if (partlen == 0)
        {
            DBG("empty path component");
            return false;
        }
        if (partlen == 1 && s[0] == '.')
        {
            DBG("'.' path component");
            return false;
        }
        if (partlen == 2 && s[0] == '.' && s[1] == '.')
        {
            DBG("'..' path component");
            return false;
        }
    }

    DBG("path is normalized");
    return true;
}

char *absolutize(const char *path)
{
    char buf[PATH_MAX + 1];

    DBG("path=%s", path);

    if (path == NULL || *path == '\0')
    {
        ERR("empty path");
        return NULL;
    }

    if (*path == '/')
    {
        DBG("already absolute path, going to normalize");
        return normalize(path);
    }

    DBG("going to getcwd");
    if (getcwd(buf, sizeof buf) == NULL)
    {
        DBG("getcwd return NULL");
        return NULL;
    }

    DBG("getcwd() return %s", buf);
    if (sizeof(buf) - strlen(buf) - 1 < strlen(path) + 1)
    {
        ERR("not enough space in buf");
        return NULL;
    }

    strcat(buf, "/");
    strcat(buf, path);
    DBG("going to normalize %s", buf);
    return normalize(buf);
}

bool is_absolute(const char *path)
{
    DBG("path=%s", path);
    return path != NULL && *path == '/';
}

bool is_subpath(const char *root, const char *path)
{
    DBG("root=%s, path=%s", root, path);

    const char *r = is_absolute(root) && is_normalized(root) ? root : absolutize(root);
    DBG("r=%s", r);
    const char *p = is_absolute(path) && is_normalized(path) ? path : absolutize(path);
    DBG("p=%s", p);

    if (r == NULL)
    {
        ERR("null root");
        return false;
    }

    if (p == NULL)
    {
        ERR("null path");
        return false;
    }

    bool ret = false;

    size_t rlen = ::strlen(r);
    size_t plen = ::strlen(p);
    DBG("rlen=%u, plen=%u", (unsigned)rlen, (unsigned)plen);
    if (rlen == 1 && *r == '/')
        ret = true;
    else if (rlen <= plen && ::strncmp(r, p, rlen) == 0 && (p[rlen] == '\0' || p[rlen] == '/'))
        ret = true;

    if (r != root)
        free((void*)r);
    if (p != path)
        free((void*)p);

    DBG("ret=%s", ret ? "true " : "false");
    return ret;
}

char *relpath(const char *root, const char *path)
{
    DBG("root=%s, path=%s", root, path);

    const char *r = is_absolute(root) && is_normalized(root) ? root : absolutize(root);
    DBG("r=%s", r);
    const char *p = is_absolute(path) && is_normalized(path) ? path : absolutize(path);
    DBG("p=%s", p);

    if (r == NULL)
    {
        ERR("null root");
        return NULL;
    }

    if (p == NULL)
    {
        ERR("null path");
        return NULL;
    }

    char *ret = NULL;

    size_t rlen = ::strlen(r);
    size_t plen = ::strlen(p);
    DBG("rlen=%u, plen=%u", (unsigned)rlen, (unsigned)plen);
    if (rlen == 1 && *r == '/')
        ret = ::strdup(p + 1);
    else if (rlen <= plen && ::strncmp(r, p, rlen) == 0 && (p[rlen] == '\0' || p[rlen] == '/'))
    {
        const char *s = p + rlen;
        size_t len = plen - rlen;
        DBG("s=%s, len=%u", s, (unsigned)len);
        if (*s == '/')
        {
            ++s;
            --len;
        }
        DBG("s=%s, len=%u", s, (unsigned)len);

        ret = (char*)malloc(len + 1);
        strncpy(ret, s, len);
        ret[len] = '\0';
    }

    if (r != root)
        free((void*)r);
    if (p != path)
        free((void*)p);

    DBG("ret=%s", ret);
    return ret;
}

char *basename(const char *path)
{
    DBG("path=%s", path);
    if (path == NULL)
    {
        ERR("path is null");
        return NULL;
    }

    if (*path == '\0')
    {
        DBG("empty path");
        return ::strdup("");
    }

    char buf[NAME_MAX + 1];

    const char *p = is_normalized(path) ? path : normalize(path);
    size_t len = ::strlen(p);
    DBG("p=%s, len=%u", p, (unsigned)len);
    if (len == 0)
        buf[0] = '\0';
    else
    {
        char *s = (char*)::memrchr(p, '/', len);
        DBG("s=%s", s);
        if (s == NULL)
        {
            ::strncpy(buf, p, len);
            buf[len] = '\0';
        }
        else
        {
            size_t partlen = p + len - s - 1;
            DBG("partlen=%u", (unsigned)partlen);
            if (partlen == 0)
                ::strcpy(buf, ".");
            else
            {
                ::strncpy(buf, s + 1, partlen);
                buf[partlen] = '\0';
            }
        }
    }

    if (p != path) ::free((void*)p);
    DBG("buf=%s", buf);
    return ::strdup(buf);
}

char *dirname(const char *path)
{
    DBG("path=%s", path);
    if (path == NULL)
    {
        ERR("path is null");
        return NULL;
    }

    char buf[PATH_MAX + 1];

    const char *p = is_normalized(path) ? path : normalize(path);
    size_t len = ::strlen(p);
    DBG("p=%s, len=%u", p, (unsigned)len);
    if (len == 0)
        ::strcpy(buf, ".");
    else
    {
        char *s = (char*)::memrchr(p, '/', len);
        DBG("s=%s", s);
        if (s == NULL)
            ::strcpy(buf, ".");
        else
        {
            ::strncpy(buf, p, s - p);
            buf[s - p] = '\0';
            if (buf[0] == '\0')
                ::strcpy(buf, "/");
        }
    }

    if (p != path) ::free((void*)p);
    DBG("buf=%s", buf);
    return ::strdup(buf);
}

} // namespace fileio
} // namespace crystax
