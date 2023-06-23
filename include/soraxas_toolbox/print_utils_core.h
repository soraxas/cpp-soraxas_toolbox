#pragma once

#include <iostream>
#include <sstream>

namespace sxs
{
    // return a pointer to a referenced ostream
    inline static std::ostream *&get_print_output_stream()
    {
        static std::ostream *ostream{&std::cout};
        return ostream;
    }

    inline static void assign_print_output_stream(std::ostream *oss)
    {
        get_print_output_stream() = oss;
    }

    inline void print_flush()
    {
        std::flush(*get_print_output_stream());
    }

    // variadic print function
    template <typename T1>
    inline void print(const T1 &first)
    {
        *get_print_output_stream() << first;
    }

    // specialisation
    template <>
    inline void print(const bool &boolean)
    {
        if (boolean)
            print("true");
        else
            print("false");
    }

    template <typename T1, typename... T2>
    inline void print(const T1 &first, const T2 &...rest)
    {
        print(first);
        print(rest...);
    }

    template <typename T1>
    inline void print_spaced(const T1 &first)
    {
        print(first);
    }

    /*
     *  Print a comma separated list of any items, with space in-between
     * */
    template <typename T1, typename... T2>
    inline void print_spaced(const T1 &first, const T2 &...rest)
    {
        print(first, " ");
        print_spaced(rest...);
    }

    /*
     *  Print an empty line
     * */
    template <typename... T>
    inline void println()
    {
        *get_print_output_stream() << std::endl;
    }

    /*
     *  Print a comma separated list of any items
     * */
    template <typename... T>
    inline void println(const T &...rest)
    {
        print(rest...);
        println();
    }

    /*
     *  Print a comma separated list of any items, with space in-between
     * */
    template <typename... T>
    inline void println_spaced(const T &...rest)
    {
        print_spaced(rest...);
        println();
    }

    class OutputStreamGuardBase
    {
        /*
         * An RAII interface for changing output stream within some scope
         */
    public:
        std::ostringstream &oss()
        {
            return get_ostream();
        }

        std::ostringstream &get_ostream()
        {
            return oss_;
        }

    protected:
        explicit OutputStreamGuardBase()
        {
        }

        std::ostringstream oss_;
    };

    class OutputStreamGuard : public OutputStreamGuardBase
    {
        /*
         * An RAII interface for changing output stream within some scope
         */
    public:
        explicit OutputStreamGuard(std::ostream &stream_to_be_swapped)
          : OutputStreamGuardBase(), stream_to_be_swapped_(stream_to_be_swapped)
        {
            // capture
            original_stream_ = stream_to_be_swapped.rdbuf(oss_.rdbuf());
        }

        ~OutputStreamGuard()
        {
            // reset
            stream_to_be_swapped_.rdbuf(original_stream_);
        }

    private:
        std::streambuf *original_stream_;
        std::ostream &stream_to_be_swapped_;
    };

    class SXSPrintOutputStreamGuard : public OutputStreamGuardBase
    {
        /*
         * An RAII interface for changing output stream within some scope
         */
    public:
        explicit SXSPrintOutputStreamGuard() : OutputStreamGuardBase()
        {
            original_stream_ = sxs::get_print_output_stream();
            sxs::get_print_output_stream() = &oss_;
        }

        ~SXSPrintOutputStreamGuard()
        {
            sxs::get_print_output_stream() = original_stream_;
        }

    protected:
        std::ostream *original_stream_;
    };

}  // namespace sxs
