#include <vector>
#include <memory>
#include <unistd.h>

// for abi namespace
#include <cxxabi.h>

#include "soraxas_toolbox/SimpleCSVWriter.h"

int main(int argc, char **argv) {

    //CSVWriterStream writer;
    //// writer.enableWriteImmediately("vv.csv");
    //writer.enableWriteImmediately("vv.csv");

    //writer.newRow() << "HI" << "how are" << "you";
    //sleep(1);
    //writer.newRow() << "HaaI" << "how are" << "you";
    //sleep(1);
    //writer.newRow() << "HaaI1" << "how are" << "you";
    //sleep(1);
    //writer.newRow() << "HaaI2" << "how are" << "you";
    //sleep(1);
    //writer.newRow() << "HaaI3" << "how are" << "you";

    //writer.addNewRow("HI", "how are", "you");
    //writer.addNewRow("HaaI", "how are", "you");
    //writer.addNewRow("HaaI1", 0.4, "you");
    //writer.addNewRow("HaaI2", "how are", "you");
    //writer.addNewRow("HaaI3", "how are", "you");

    //std::cout << writer << std::endl;

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
