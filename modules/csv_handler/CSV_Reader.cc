// CSV_Reader.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004-2009 University Corporation for Atmospheric Research
// Author: Stephan Zednik <zednik@ucar.edu> and Patrick West <pwest@ucar.edu>
// and Jose Garcia <jgarcia@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301

// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//	zednik      Stephan Zednik <zednik@ucar.edu>
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include "CSV_Reader.h"
#include "CSV_Utils.h"
#include "BESUtil.h"

using std::ostream;
using std::fstream;
using std::endl;
using std::ios;
using std::string;
using std::vector;

CSV_Reader::CSV_Reader(): _row_number(0) {
    _stream_in = new fstream();
}

CSV_Reader::~CSV_Reader() {
    if (_stream_in) {
        if (_stream_in->is_open()) {
            _stream_in->close();
        }
        delete _stream_in;
        _stream_in = 0;
    }
}

bool
CSV_Reader::open(const string &filepath) {
    bool ret = false;
    _filepath = filepath;
    _stream_in->open(filepath.c_str(), fstream::in);
    if (!(_stream_in->fail()) && _stream_in->is_open()) {
        _row_number = 0;
        ret = true;
    }
    return ret;
}

bool
CSV_Reader::close() const {
    bool ret = false;
    if (_stream_in) {
        _stream_in->close();
        if (!(_stream_in->bad()) && !(_stream_in->is_open())) {
            ret = true;
        }
    }
    return ret;
}

bool
CSV_Reader::eof() const {
    return _stream_in->eof();
}

void
CSV_Reader::reset() {
    _row_number = 0;
    _stream_in->seekg(ios::beg);
}


void
CSV_Reader::get(vector<string> &row) {
    string line;

    // Read and ignore empty lines of comment lines. Comment lines
    // must start with a '#'. Pretty primitive; if more is needed,
    // add a function to test for a comment line. jhrg 3/11/21
    do {
        getline(*_stream_in, line);
        // when we reach EOF, line.length() is zero and that condition
        // (i.e., when row is zero in CSV_Utils::split() below) signals
        // EOF to this handler. jhrg 3/11/21
    } while(!_stream_in->eof() && (line.length() == 0 || line[0] == '#'));

    CSV_Utils::split(line, ',', row);
    _row_number++;
}

void
CSV_Reader::dump(ostream &strm) const {
    strm << BESIndent::LMarg << "CSV_Reader::dump - ("
         << (void *) this << ")" << endl;
    BESIndent::Indent();
    if (_stream_in) {
        strm << BESIndent::LMarg << "File " << _filepath << " is open" << endl;
        strm << BESIndent::LMarg << "Current row " << _row_number << endl;
    }
    else {
        strm << BESIndent::LMarg << "No stream opened at this time" << endl;
    }
    BESIndent::UnIndent();
}
