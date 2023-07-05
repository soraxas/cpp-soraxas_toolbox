/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Tin Yiu Lai (@soraxas)
 *
 * This file is part of the project soraxas_toolbox, a collections of utilities
 * for developing c++ applications.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <memory>
#include <unistd.h>
#include <vector>

// for abi namespace
#include "soraxas_toolbox/SimpleCSVWriter.h"
#include "soraxas_toolbox/path.h"

#include <cxxabi.h>

int main(int argc, char **argv)
{
    // CSVWriterStream writer;
    //// writer.enableWriteImmediately("vv.csv");
    // writer.enableWriteImmediately("vv.csv");

    // writer.newRow() << "HI" << "how are" << "you";
    // sleep(1);
    // writer.newRow() << "HaaI" << "how are" << "you";
    // sleep(1);
    // writer.newRow() << "HaaI1" << "how are" << "you";
    // sleep(1);
    // writer.newRow() << "HaaI2" << "how are" << "you";
    // sleep(1);
    // writer.newRow() << "HaaI3" << "how are" << "you";

    // writer.addNewRow("HI", "how are", "you");
    // writer.addNewRow("HaaI", "how are", "you");
    // writer.addNewRow("HaaI1", 0.4, "you");
    // writer.addNewRow("HaaI2", "how are", "you");
    // writer.addNewRow("HaaI3", "how are", "you");

    // std::cout << writer << std::endl;

    CSVInstantWriter writer("test.csv");
    writer.addNewRow("HI", "how are", "you");
    writer.addNewRow("HaaI", "how are", "you");
    writer.addNewRow("HaaI1", 0.4, "you", "99999999", 898893242893);
    writer.addNewRow("HaaI2", "how are", "you");
    writer.addNewRow("HaaI3", "how are", "you");

    // writer.writeToFile("aa.csv");
    //     writer.writeToFile("aav.csv", true);
    //
    //

    std::cout << sxs::get_home_dir() << std::endl;
}
