//
// Created by Kodi Neumiller on 5/5/21.
//
#include <iostream>
#include <sstream>

#include <DAS.h>

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

//FIXME: You may have to limit the number of pybind modules to 1 per cpp file...
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
        .def("print", [](const libdap::DAS &a) {
            std::ostringstream oss;
            const_cast<libdap::DAS&>(a).print(oss, false);
            return oss.str();
        })

        // This does not do what I'd expect. jhrg 7/23/21
        .def("ref", [](const libdap::DAS &a) {
            return a;
        }, py::return_value_policy::reference)

        // Why does this not print the '\n' stuff like the 'print' method above? jhrg 7/23/21
        .def("__repr__", [](const libdap::DAS &a) {
            std::ostringstream oss;
            const_cast<libdap::DAS&>(a).print(oss, false);
            return oss.str();
        });
#if 0
        m.def("__repr__", [](const libdap::DAS &a) {
            std::ostringstream oss;
            const_cast<libdap::DAS&>(a).print(oss, false);
            return oss.str();
        });

        m.def("get_das", [](const libdap::DAS &a) {
            return a;
        }, py::return_value_policy::reference);



        m.def("redirect_func", []() {
            py::scoped_ostream_redirect stream(
                std::cout,                               // std::ostream&
                py::module_::import("sys").attr("stdout") // Python output
            );
            //call_redirect_func();
            });
#endif
}