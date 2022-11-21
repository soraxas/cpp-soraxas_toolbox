#pragma once

#include <iostream>
#include <string>

namespace simple_logger
{

    enum log_level_t
    {
        LOG_NOTHING,
        LOG_CRITICAL,
        LOG_ERROR,
        LOG_WARNING,
        LOG_INFO,
        LOG_DEBUG
    };

    struct GlobalData
    {
        log_level_t GLOBAL_LEVEL = LOG_NOTHING;
        std::string prefix = "";
    };

    inline GlobalData &get_global_data()
    {
        static GlobalData data;
        return data;
    }

    inline void set_verbosity(log_level_t level = LOG_DEBUG)
    {
        get_global_data().GLOBAL_LEVEL = level;
    }

    inline void set_silence()
    {
        get_global_data().GLOBAL_LEVEL = LOG_NOTHING;
    }

    class DummyStreamBuf : public std::streambuf
    {
    };

    inline std::ostream &log(log_level_t thereshold)
    {
        static DummyStreamBuf dummystreambuf;
        static std::ostream dummystream(&dummystreambuf);
        std::ostream &stream =
            (get_global_data().GLOBAL_LEVEL >= thereshold) ? std::cerr : dummystream;

        stream << get_global_data().prefix;

        return stream;
    }
}  // namespace simple_logger