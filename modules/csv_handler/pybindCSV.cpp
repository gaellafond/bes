//
// Created by Kodi Neumiller on 5/5/21.
//
#include <iostream>
#include <sstream>

#include "config.h"

#include <DAS.h>

#include <CSVRequestHandler.h>

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

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

PYBIND11_MODULE(pybindCSV, m) {
    py::class_<CSVRequestHandler>(m, "CSVhandler")
        .def(py::init<const CSVRequestHandler &>());
        py::print("test");

        .def("dump", py::overload_cast<std::ostream>(&bes::BESDataHandlerInterface::csv_build_das))
        .def("csv_build_das", py::overload_cast<BESDataHandlerInterface>(&bes::BESDataHandlerInterface::csv_build_das));
}