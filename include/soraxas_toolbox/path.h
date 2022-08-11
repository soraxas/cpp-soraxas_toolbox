//
// Created by tin on 11/08/22.
//

#ifndef SXS_PATH_UTILS_H
#define SXS_PATH_UTILS_H

#include "future.h"

#include <pwd.h>     // for getting home dir
#include <unistd.h>  // for getting home dir

namespace sxs
{
    inline std::string get_home_dir()
    {
        /*
         * Return the current home directory
         */
        const char *homedir;

        // Check $HOME environment variable first before retrieving user's
        // homedir
        if ((homedir = getenv("HOME")) == NULL)
        {
            homedir = getpwuid(getuid())->pw_dir;
        }

        return homedir;
    }
}  // namespace sxs

#endif  // SXS_PATH_UTILS_H
