// This file is part of hdf5_handler a HDF5 file handler for the OPeNDAP
// data server.

// Copyright (c) 2007-2016 The HDF Group, Inc. and OPeNDAP, Inc.
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
// You can contact The HDF Group, Inc. at 1800 South Oak Street,
// Suite 203, Champaign, IL 61820  
////////////////////////////////////////////////////////////////////////////////
/// \file h5get.h
/// Helper functions to generate DDS/DAS/DODS for the default option.
///

///
/// 
/// \author Hyo-Kyung Lee <hyoklee@hdfgroup.org>
/// \author Muqun Yang <ymuqun@hdfgroup.org>


#ifndef _H5GET_H
#define _H5GET_H
#include "hdf5_handler.h"
#include "h5common.h"
#include "h5apicompatible.h"

bool check_h5str(hid_t);

void close_fileid(hid_t fid);

hid_t get_attr_info(hid_t dset, int index, bool, DSattr_t * attr_inst, bool*);

std::string get_dap_type(hid_t type,bool);

void get_dataset_dmr(const hid_t file_id, hid_t pid, const std::string &dname, DS_t * dt_inst_ptr,bool has_dimscale, bool &is_pure_dims,std::vector<link_info_t> &);
void get_dataset(hid_t pid, const std::string &dname, DS_t * dt_inst_ptr);

hid_t get_fileid(const char *filename);

std::string print_attr(hid_t type, int loc, void *sm_buf);

D4AttributeType daptype_strrep_to_dap4_attrtype(const std::string & s);

//static BaseType *Get_bt(const string &vname,
libdap::BaseType *Get_bt(const std::string &vname,const std::string &var_path,
                        const std::string &dataset,
                        hid_t datatype,bool is_dap4);

//static Structure *Get_structure(const string &varname,
libdap::Structure *Get_structure(const std::string &varname,const std::string &var_path,
                                const std::string &dataset,
                                hid_t datatype,bool is_dap4);

bool check_dimscale(hid_t fid);
bool has_dimscale_attr(hid_t dataset);
void obtain_dimnames(const hid_t file_id, hid_t dset,int, DS_t*dt_inst_ptr, std::vector<link_info_t>&);

void write_vlen_str_attrs(hid_t attr_id,hid_t ty_id, const DSattr_t *, libdap::D4Attribute *d4_attr, libdap::AttrTable* d2_attr,bool is_dap4);

bool check_str_attr_value(hid_t attr_id,hid_t atype_id,const string & value_to_compare,bool is_substr);

std::string obtain_shortest_ancestor_path(const std::vector<std::string> &);

#endif                          //_H5GET_H
