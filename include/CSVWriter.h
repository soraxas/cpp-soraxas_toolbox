#pragma once

// based on https://github.com/al-eax/CSVWriter.git

#ifndef CSVWRITER_H
#define CSVWRITER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <typeinfo>

#include "soraxas_cpp_toolbox.h"

class CSVWriterStream
{
    public:
        CSVWriterStream(std::string seperator=",", int numberOfColums=-1){
            firstRow_ = true;
            seperator_ = seperator;
            columnNum_ = numberOfColums;
            valueCount_ = 0;
        }

        CSVWriterStream(int numberOfColums): CSVWriterStream() {
            columnNum_ = numberOfColums;
        }

        CSVWriterStream& add(const char *str){
            return add(std::string(str));
        }

        // CSVWriterStream& add(char *str){
        //     return add(std::string(str));
        // }

        CSVWriterStream& add(std::string str){
            //if " character was found, escape it
            size_t position = str.find("\"",0);
            bool foundQuotationMarks = position != std::string::npos;
            while(position != std::string::npos){
                str.insert(position,"\"");
                position = str.find("\"",position + 2);
            }
            if(foundQuotationMarks){
                str = "\"" + str + "\"";
            }else if(str.find(seperator_) != std::string::npos){
                //if seperator was found and string was not escapted before, surround string with "
                str = "\"" + str + "\"";
            }
            return add<std::string>(str);
        }

        template<typename T>
        CSVWriterStream& add(T str){
            if(columnNum_ > -1){
                //if autoNewRow is enabled, check if we need a line break
                if(valueCount_ == columnNum_ ){
                    newRow();
                }
            }
            if(valueCount_ > 0)
                ss_ << seperator_;
            ss_ << str;
            valueCount_++;

            return *this;
        }

        template<typename T>
        CSVWriterStream& operator<<(const T& t){
            return add(t);
        }

        void operator+=(CSVWriterStream &csv){
            ss_ << std::endl << csv;
        }

        std::string toString(){
            return ss_.str();
        }

        friend std::ostream& operator<<(std::ostream& os, CSVWriterStream & csv){
            return os << csv.toString();
        }

        CSVWriterStream& newRow(){
            if(!firstRow_ || columnNum_ > -1){
                ss_ << std::endl;
            }else{
                //if the row is the first row, do not insert a new line
                firstRow_ = false;
            }
            if (writeImmediately_)
            {
                // write to file without auto-adding newline (as it will be handled on a per-line basis)
                writeToFile(writeImmediatelyFilename_, true, false);
                ss_.str(""); // clear output
            }
            valueCount_ = 0;
            return *this;
        }

        bool writeToFile(std::string filename, bool append=false, bool add_newline=true){
            std::ofstream file;
            auto open_method = std::ios::trunc;
            if(append)
                open_method = std::ios::app;
            file.open(filename.c_str(), std::ios::out | open_method);
            if(!file.is_open())
                return false;
            if (append && add_newline)
                file << std::endl;
            file << toString();
            file.close();
            return file.good();
        }

        bool enableWriteImmediately(std::string filename, bool append=false){
            // when enabled, the internal string stream will be flushed (cleared)
            // when writing a new row.
            writeImmediately_ = true;
            writeImmediatelyFilename_ = filename;
            if (!append) {
                // clear file content (by opening it)
                std::ofstream file;
                file.open(filename.c_str(), std::ios::out | std::ios::trunc);
                if (!file.is_open())
                    return false;
                file.close();
                return file.good();
            }
            return true;
        }

        void disableWriteImmediately(){
            writeImmediately_ = false;
        }

        void enableAutoNewRow(int numberOfColumns){
            columnNum_ = numberOfColumns;
        }

        void disableAutoNewRow(){
            columnNum_ = -1;
        }

        void initialise_timer() {
            timer = std::make_unique<Timer>();
        }

        double get_elapsed_time() {
            if (!timer)
                initialise_timer();
            return timer->elapsed();
        }
    protected:
        bool firstRow_;
        bool writeImmediately_;
        std::string writeImmediatelyFilename_;
        std::string seperator_;
        int columnNum_;
        int valueCount_;
        std::stringstream ss_;
    public:
        std::unique_ptr<Timer> timer;

};

class CSVInstantWriter : CSVWriterStream
{
public:
    CSVInstantWriter(std::string filename, bool append=false, std::string seperator=","): 
        CSVWriterStream(","), writeImmediatelyFilename_(filename)
    {
        if (!append)
        {
            // clear file content (by opening it)
            std::ofstream file;
            file.open(writeImmediatelyFilename_.c_str(), std::ios::out | std::ios::trunc);
            file.close();
        }
    }

    void initialise_timer() {
        CSVWriterStream::initialise_timer();
    }

    double get_elapsed_time() {
        return CSVWriterStream::get_elapsed_time();
    }

    template <typename T, typename... Args>
    void addNewRow(T t, Args... args)
    {
        add(t);
        addNewRow(args...);
    }
    template <typename T>
    void addNewRow(T t)
    {
        add(t);
        bool write_newline = true;
        if(firstRow_) {
            //if the row is the first row, do not insert a new line
            firstRow_ = false;
            write_newline = false;
        }
        writeToFile(writeImmediatelyFilename_, true, write_newline);
        ss_.str(""); // clear output
        valueCount_ = 0;
    }
protected:
    std::string writeImmediatelyFilename_;
private:
    void enableAutoNewRow(int numberOfColumns) = delete;
    void disableAutoNewRow() = delete;

public:
    std::unique_ptr<Timer> timer;
};

#endif // CSVWRITER_H
