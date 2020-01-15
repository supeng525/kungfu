//
// Created by qlu on 2019/2/19.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <kungfu/yijinjing/io.h>

#include "marketdata_ufx.h"
#include "trader_ufx.h"

namespace py = pybind11;

using namespace kungfu::yijinjing::data;
using namespace kungfu::wingchun::ufx;

PYBIND11_MODULE(kfext_ufx, m)
{
    py::class_<MarketDataUFX, kungfu::practice::apprentice, std::shared_ptr<MarketDataUFX>>(m, "MD")
            .def(py::init<bool, locator_ptr, const std::string &>())
            .def("run", &MarketDataUFX::run, py::call_guard<py::gil_scoped_release>());

    py::class_<TraderUFX, kungfu::practice::apprentice, std::shared_ptr<TraderUFX>>(m, "TD")
            .def(py::init<bool, locator_ptr, const std::string &, const std::string &>())
            .def("run", &TraderUFX::run, py::call_guard<py::gil_scoped_release>());
}
