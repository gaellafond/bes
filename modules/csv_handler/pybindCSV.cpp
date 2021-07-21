//
// Created by Kodi Neumiller on 5/5/21.
//
#include <iostream>
#include <sstream>

#include "config.h"

#include <DAS.h>

#include <CSVRequestHandler.h>
#include <CSVDAS.h>

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>

// NB Build using:
// c++ -I/Users/kodi/src/hyrax/bes/modules/csv_handler -I/Users/kodi/src/hyrax/bes/dispatch
// -I/Users/kodi/src/hyrax/libdap4 -O3 -Wall -shared -std=c++14 -undefined dynamic_lookup
// $(python3 -m pybind11 --includes) pybindCSV.cpp -o pybindCSV$(python3-config --extension-suffix)
// -L$prefix/lib -ldap

namespace py = pybind11;

PYBIND11_MODULE(pybindCSV, m) {
    py::class_<CSVRequestHandler>(m, "CSVHandler")
        .def(py::init<const std::string &>())
        .def("dump", &CSVRequestHandler::dump, "dump")
        .def("csv_build_das", &CSVRequestHandler::csv_build_das, "build the das")
        //Where is "csv_read_attributes" located?
        .def("csv_read_attributes", &::csv_read_attributes, "read the csv attributes");

    //.def("csv_build_das", py::overload_cast<BESDataHandlerInterface>(&CSVRequestHandler::csv_build_das));
    //.def("csv_read_attributes", &csv_read_attributes);
    py::print("test");
}

//FIXME: You may have to limit the number of pybind modules to 1 per cpp file...
#if 0
PYBIND11_MODULE(pybindDAS, m) {
    py::class_<libdap::DAS>(m, "DAS")
        //init() is a convenience function that takes the types of a constructor’s
        // parameters as template arguments and wraps the corresponding constructor
        .def(py::init<>())
        .def(py::init<const libdap::DAS &>())
        .def("get_size", &libdap::DAS::get_size, "get the size")
        //if there are default arguments use py::arg()
        .def("parse", py::overload_cast<string>(&libdap::DAS::parse), "parse with string")
        .def("parse", py::overload_cast<int>(&libdap::DAS::parse), "parse with int")
        .def("parse", py::overload_cast<FILE*>(&libdap::DAS::parse), "parse with FILE")
        .def("print", py::overload_cast<FILE*, bool>(&libdap::DAS::print), "print das with FILE out")
        .def("print", py::overload_cast<ostream&, bool>(&libdap::DAS::print), "print das with ostream")
        //print statement
        .def("__repr__", [](const libdap::DAS &a) {
            std::ostringstream oss;
            const_cast<libdap::DAS&>(a).print(oss, false);
            return oss.str();
        });

        m.def("redirect_func", []() {
            py::scoped_ostream_redirect stream(
                    std::cout,                               // std::ostream&
                    py::module_::import("sys").attr("stdout") // Python output
            );
        //call_redirect_func();
        });
}
#endif