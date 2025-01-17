// This file is part of hdf5_handler: an HDF5 file handler for the OPeNDAP
// data server.

// Copyright (c) 2011-2016 The HDF Group, Inc. and OPeNDAP, Inc.
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
/// \file h5commoncfdap.cc
/// \brief Functions to generate DDS and DAS for one object(variable). 
///
/// 
/// \author Muqun Yang <myang6@hdfgroup.org>
///
////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <libdap/InternalErr.h>
#include <BESDebug.h>

#include "HDF5RequestHandler.h"
#include "h5cfdaputil.h"
#include "h5gmcfdap.h"
#include "HDF5CFByte.h"
#include "HDF5CFInt8.h"
#include "HDF5CFUInt16.h"
#include "HDF5CFInt16.h"
#include "HDF5CFUInt32.h"
#include "HDF5CFInt32.h"
#include "HDF5CFFloat32.h"
#include "HDF5CFFloat64.h"
#include "HDF5CFInt64.h"
#include "HDF5CFUInt64.h"
#include "HDF5CFStr.h"
#include "HDF5CFArray.h"
#include "HDF5CFGeoCF1D.h"
#include "HDF5CFGeoCFProj.h"

//#include "HDF5Int64.h"
#include "HDF5CFUtil.h"

using namespace std;
using namespace libdap;
using namespace HDF5CF;

// TODO In the code below, reduce duplication using a template and use unique_ptr
//  to remove memory leak inside catch(...)/throw block. jhrg 3/9/22
// Generate DDS from one variable
void gen_dap_onevar_dds(DDS &dds, const HDF5CF::Var* var, const hid_t file_id, const string & filename)
{

    BESDEBUG("h5", "Coming to gen_dap_onevar_dds()  "<<endl);
    const vector<HDF5CF::Dimension *>& dims = var->getDimensions();

    if (dims.empty()) {
        // Adding 64-bit integer support for DMR 
        if (H5INT64 == var->getType() || H5UINT64 == var->getType()){
            DMR * dmr = HDF5RequestHandler::get_dmr_64bit_int();
            if(dmr == nullptr)
                return;
            else {
                D4Group* root_grp = dmr->root();
                if(H5INT64 == var->getType()) {
                    HDF5CFInt64 *sca_int64 = nullptr;
                    try {
                        sca_int64 = new HDF5CFInt64(var->getNewName(), var->getFullPath(), filename);
                    }
                    catch (...) {
                        string error_message = "Cannot allocate the HDF5CFInt64: " + error_message;
                        throw InternalErr(__FILE__, __LINE__, error_message);
                    }
                    sca_int64->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4_int64(var,sca_int64);
                    root_grp->add_var_nocopy(sca_int64);
 
                }
                else if(H5UINT64 == var->getType()) {
                    HDF5CFUInt64 *sca_uint64 = nullptr;
                    try {
                        sca_uint64 = new HDF5CFUInt64(var->getNewName(), var->getFullPath(), filename);
                    }
                    catch (...) {
                        throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFInt64.");
                    }
                    sca_uint64->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4_int64(var,sca_uint64);
                    root_grp->add_var_nocopy(sca_uint64);
 
                }

            }
        }
        else if (H5FSTRING == var->getType() || H5VSTRING == var->getType()) {
            HDF5CFStr *sca_str = nullptr;
            try {
                sca_str = new HDF5CFStr(var->getNewName(), filename, var->getFullPath());
            }
            catch (...) {
                throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFStr.");
            }
            dds.add_var(sca_str);
            delete sca_str;
        }
        else {
            switch (var->getType()) {

            case H5UCHAR: {
                HDF5CFByte * sca_uchar = nullptr;
                try {
                    sca_uchar = new HDF5CFByte(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFByte.");
                }
                dds.add_var(sca_uchar);
                delete sca_uchar;

            }
                break;
            case H5CHAR:
            case H5INT16: {
                HDF5CFInt16 * sca_int16 = nullptr;
                try {
                    sca_int16 = new HDF5CFInt16(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFInt16.");
                }
                dds.add_var(sca_int16);
                delete sca_int16;
            }
                break;
            case H5UINT16: {
                HDF5CFUInt16 * sca_uint16 = nullptr;
                try {
                    sca_uint16 = new HDF5CFUInt16(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFUInt16.");
                }
                dds.add_var(sca_uint16);
                delete sca_uint16;
            }
                break;
            case H5INT32: {
                HDF5CFInt32 * sca_int32 = nullptr;
                try {
                    sca_int32 = new HDF5CFInt32(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFInt32.");
                }
                dds.add_var(sca_int32);
                delete sca_int32;
            }
                break;
            case H5UINT32: {
                HDF5CFUInt32 * sca_uint32 = nullptr;
                try {
                    sca_uint32 = new HDF5CFUInt32(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFUInt32.");
                }
                dds.add_var(sca_uint32);
                delete sca_uint32;
            }
                break;
            case H5FLOAT32: {
                HDF5CFFloat32 * sca_float32 = nullptr;
                try {
                    sca_float32 = new HDF5CFFloat32(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFFloat32.");
                }
                dds.add_var(sca_float32);
                delete sca_float32;
            }
                break;
            case H5FLOAT64: {
                HDF5CFFloat64 * sca_float64 = nullptr;
                try {
                    sca_float64 = new HDF5CFFloat64(var->getNewName(), var->getFullPath(), filename);
                }
                catch (...) {
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFFloat64.");
                }
                dds.add_var(sca_float64);
                delete sca_float64;

            }
                break;
            default:
                throw InternalErr(__FILE__, __LINE__, "unsupported data type.");
            }
        }
    }

    else {

        // 64-bit integer support
        // DMR CHECK
        bool dap4_int64 = false;
        if(var->getType() == H5INT64 || var->getType()==H5UINT64) { 
            DMR * dmr = HDF5RequestHandler::get_dmr_64bit_int();
            if(dmr == nullptr)
                return;
            else 
                dap4_int64 = true;
        }

#if 0
            else {
                D4Group* root_grp = dmr->root();
                BaseType *bt = nullptr;
                bt = new(HDF5Int64)(var->getNewName(),var->getFullPath(),filename);
                bt->transform_to_dap4(root_grp,root_grp);
                delete bt;
                return;
            }
#endif
        BaseType *bt = nullptr;

        if(true == dap4_int64) {
            if(var->getType() == H5INT64)
                bt = new(HDF5CFInt64)(var->getNewName(),var->getFullPath());
            else if(var->getType() == H5UINT64)
                bt = new(HDF5CFUInt64)(var->getNewName(),var->getFullPath());
        }

        else {
        switch (var->getType()) {
            // TODO Remove extra ';' jhrg 3/9/22
#define HANDLE_CASE(tid,type)                                  \
            case tid:                                           \
                bt = new (type)(var->getNewName(),var->getFullPath()); \
            break;
        HANDLE_CASE(H5FLOAT32, HDF5CFFloat32)
        HANDLE_CASE(H5FLOAT64, HDF5CFFloat64)
        HANDLE_CASE(H5CHAR, HDF5CFInt16)
        HANDLE_CASE(H5UCHAR, HDF5CFByte)
        HANDLE_CASE(H5INT16, HDF5CFInt16)
        HANDLE_CASE(H5UINT16, HDF5CFUInt16)
        HANDLE_CASE(H5INT32, HDF5CFInt32)
        HANDLE_CASE(H5UINT32, HDF5CFUInt32)
        HANDLE_CASE(H5FSTRING, Str)
        HANDLE_CASE(H5VSTRING, Str)
        default:
            throw InternalErr(__FILE__, __LINE__, "unsupported data type.");
#undef HANDLE_CASE
        }
        }

        vector<size_t> dimsizes;
        dimsizes.resize(var->getRank());
        for (int i = 0; i < var->getRank(); i++)
            dimsizes[i] = (dims[i])->getSize();

        HDF5CFArray *ar = nullptr;
        try {
            ar = new HDF5CFArray(var->getRank(), file_id, filename, var->getType(), dimsizes, var->getFullPath(),
                var->getTotalElems(), CV_UNSUPPORTED, false, var->getCompRatio(), false,var->getNewName(), bt);
        }
        catch (...) {
            delete bt;
            throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFStr.");
        }

        for (auto it_d = dims.begin(); it_d != dims.end(); ++it_d) {
            if ("" == (*it_d)->getNewName())
                ar->append_dim((*it_d)->getSize());
            else
                ar->append_dim((*it_d)->getSize(), (*it_d)->getNewName());
        }

        // When handling DAP4 CF, we need to generate dmr for 64-bit integer separately.
        if(dap4_int64 == true) {
            DMR * dmr = HDF5RequestHandler::get_dmr_64bit_int();
            D4Group* root_grp = dmr->root();
            // Dimensions need to be translated.
            BaseType* d4_var = ar->h5cfdims_transform_to_dap4_int64(root_grp);
            // Attributes.
            map_cfh5_var_attrs_to_dap4_int64(var,d4_var);
            root_grp->add_var_nocopy(d4_var);
        }
        else 
            dds.add_var(ar);

        delete bt;
        delete ar;
    }

    return;

}

// Currently, only when the datatype of fillvalue is not the same as the datatype of the variable,
// special attribute handling is needed.
bool need_special_attribute_handling(const HDF5CF::Attribute* attr, const HDF5CF::Var* var)
{
    return ((("_FillValue" == attr->getNewName()) && (var->getType() != attr->getType())) ? true : false);
}

// Currently, we only handle the case when the datatype of _FillValue is not the same as the variable datatype.
void gen_dap_special_oneobj_das(AttrTable*at, const HDF5CF::Attribute* attr, const HDF5CF::Var* var)
{

    BESDEBUG("h5", "Coming to gen_dap_special_oneobj_das()  "<<endl);
    if (attr->getCount() != 1) throw InternalErr(__FILE__, __LINE__, "FillValue attribute can only have one element.");

    H5DataType var_dtype = var->getType();
    if ((true == HDF5RequestHandler::get_fillvalue_check()) 
        && (false == is_fvalue_valid(var_dtype, attr))) {
        string msg = "The attribute value is out of the range.\n";
        msg += "The variable name: " + var->getNewName() + "\n";
        msg += "The attribute name: " + attr->getNewName() + "\n";
        msg += "The error occurs inside the gen_dap_special_oneobj_das function in h5commoncfdap.cc.";
        throw InternalErr(msg);
    }
    string print_rep = HDF5CFDAPUtil::print_attr(attr->getType(), 0, (void*) (&(attr->getValue()[0])));
    at->append_attr(attr->getNewName(), HDF5CFDAPUtil::print_type(var_dtype), print_rep);
}

// Check if this fill value is in the valid datatype range when the fillvalue datatype is changed to follow the CF
bool is_fvalue_valid(H5DataType var_dtype, const HDF5CF::Attribute* attr)
{

    BESDEBUG("h5", "Coming to is_fvalue_valid()  "<<endl);
    bool ret_value = true;
    // We only check 8-bit and 16-bit integers. 
    switch (attr->getType()) {
    case H5CHAR: {
        signed char final_fill_value = *((signed char*) ((void*) (&(attr->getValue()[0]))));
        if ((var_dtype == H5UCHAR) && (final_fill_value<0)) 
            ret_value = false;
        return ret_value;

    }
    case H5INT16: {
        short final_fill_value = *((short*) ((void*) (&(attr->getValue()[0]))));
        if ((var_dtype == H5UCHAR) &&(final_fill_value > 255 || final_fill_value < 0)) 
            ret_value = false;
        
        // No need to check the var_dtype==H5CHAR case since it is mapped to int16.
        else if ((var_dtype == H5UINT16) && (final_fill_value < 0)) 
            ret_value = false;
        return ret_value;
    }
    case H5UINT16: {
        unsigned short final_fill_value = *((unsigned short*) ((void*) (&(attr->getValue()[0]))));
        if ((var_dtype == H5UCHAR) &&(final_fill_value > 255)) {
            ret_value = false;
        }
        else if ((var_dtype == H5INT16) && (final_fill_value >32767)){
            ret_value = false;
        }
        return ret_value;

    }
        // We are supposed to check the case when the datatype of fillvalue is unsigned char.
        // However, since the variable type signed char is always mapped to int16, so there 
        // will never be an overflow case(the signed char case is the only possible one). 
        // Still the data producer should not do this. We will not check this in the handler.KY 2016-03-04
#if 0
        case H5UCHAR:
        {
            unsigned char final_fill_value = *((unsigned char*)((void*)(&(attr->getValue()[0]))));
            if(var_dtype == H5CHAR) {
                if(final_fill_value >127)
                ret_value = false;
            }
            return ret_value;
        }

    case H5UCHAR:
    case H5INT32:
    case H5UINT32:
#endif

    default:
        return ret_value;
    }

}
// Leave the old code for the time being. KY 2015-05-07
#if 0
void gen_dap_special_oneobj_das(AttrTable*at, const HDF5CF::Attribute* attr,const HDF5CF::Var* var) {

    if (attr->getCount() != 1)
    throw InternalErr(__FILE__,__LINE__,"FillValue attribute can only have one element.");

    H5DataType var_dtype = var->getType();
    switch(var_dtype) {

        case H5UCHAR:
        {
            unsigned char final_fill_value = *((unsigned char*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;

        case H5CHAR:
        {
            // Notice HDF5 native char maps to DAP int16. 
            short final_fill_value = *((short*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;
        case H5INT16:
        {
            short final_fill_value = *((short*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;
        case H5UINT16:
        {
            unsigned short final_fill_value = *((unsigned short*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;

        case H5INT32:
        {
            int final_fill_value = *((int*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;
        case H5UINT32:
        {
            unsigned int final_fill_value = *((unsigned int*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;
        case H5FLOAT32:
        {
            float final_fill_value = *((float*)((void*)(&(attr->getValue()[0]))));
//            memcpy(&(attr->getValue()[0]),(void*)(&final_fill_value),sizeof(float));
//cerr<<"final_fill_value is "<<final_fill_value <<endl;
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;
        case H5FLOAT64:
        {
            double final_fill_value = *((double*)((void*)(&(attr->getValue()[0]))));
            print_rep = HDF5CFDAPUtil::print_attr(var_dtype,0,(void*)&final_fill_value);
        }
        break;
        default:
        throw InternalErr(__FILE__,__LINE__,"unsupported data type.");
    }

    at->append_attr(attr->getNewName(), HDF5CFDAPUtil::print_type(var_dtype), print_rep);
}
#endif

// Generate DAS from one variable
void gen_dap_oneobj_das(AttrTable*at, const HDF5CF::Attribute* attr, const HDF5CF::Var *var)
{

    BESDEBUG("h5", "Coming to gen_dap_oneobj_das()  "<<endl);
    // DMR support for 64-bit integer
    if (H5INT64 == attr->getType() || H5UINT64 == attr->getType()) {
        // TODO: Add code to tackle DMR for the variable datatype that is not 64-bit integer. 
        return;

    }
    else if ((H5FSTRING == attr->getType()) || (H5VSTRING == attr->getType())) {
        gen_dap_str_attr(at, attr);
    }
    else {

        if (nullptr == var) {

            // HDF5 Native Char maps to DAP INT16(DAP doesn't have the corresponding datatype), so needs to
            // obtain the mem datatype. 
            size_t mem_dtype_size = (attr->getBufSize()) / (attr->getCount());
            H5DataType mem_dtype = HDF5CFDAPUtil::get_mem_dtype(attr->getType(), mem_dtype_size);

            for (unsigned int loc = 0; loc < attr->getCount(); loc++) {
                string print_rep = HDF5CFDAPUtil::print_attr(mem_dtype, loc, (void*) &(attr->getValue()[0]));
                at->append_attr(attr->getNewName(), HDF5CFDAPUtil::print_type(attr->getType()), print_rep);
            }

        }

        else {

            // The datatype of _FillValue attribute needs to be the same as the variable datatype for an netCDF C file.
            // To make OPeNDAP's netCDF file out work, we need to change the attribute datatype of _FillValue to be the
            // same as the variable datatype if they are not the same. An OMI-Aura_L2-OMUVB file has such a case.
            // The datatype of "TerrainHeight" is int32 but the datatype of the fillvalue is int16. 
            // KY 2012-11-20
            bool special_attr_handling = need_special_attribute_handling(attr, var);
            if (true == special_attr_handling) {
                gen_dap_special_oneobj_das(at, attr, var);
            }

            else {

                // HDF5 Native Char maps to DAP INT16(DAP doesn't have the corresponding datatype), so needs to
                // obtain the mem datatype. 
                size_t mem_dtype_size = (attr->getBufSize()) / (attr->getCount());
                H5DataType mem_dtype = HDF5CFDAPUtil::get_mem_dtype(attr->getType(), mem_dtype_size);

                for (unsigned int loc = 0; loc < attr->getCount(); loc++) {
                    string print_rep = HDF5CFDAPUtil::print_attr(mem_dtype, loc, (void*) &(attr->getValue()[0]));
                    at->append_attr(attr->getNewName(), HDF5CFDAPUtil::print_type(attr->getType()), print_rep);
                }
            }
        }
    }
}

// TODO Use a template function in the switch() below. jhrg 3/9/22
// Generate DMR from one variable                                                                           
void gen_dap_onevar_dmr(libdap::D4Group* d4_grp, const HDF5CF::Var* var, const hid_t file_id, const string & filename) {

    BESDEBUG("h5", "Coming to gen_dap_onevar_dmr()  "<<endl);

    const vector<HDF5CF::Dimension *>& dims = var->getDimensions();

    if (dims.empty()) {

        if (H5FSTRING == var->getType() || H5VSTRING == var->getType()) {
            HDF5CFStr *sca_str = nullptr;
            try {
                sca_str = new HDF5CFStr(var->getNewName(), filename, var->getFullPath());
                sca_str->set_is_dap4(true);
                map_cfh5_var_attrs_to_dap4(var,sca_str);
            }
            catch (...) {
                delete sca_str;
                throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFStr.");
            }
            d4_grp->add_var_nocopy(sca_str);
        }
        else {
            switch (var->getType()) {

            case H5UCHAR: {
                HDF5CFByte * sca_uchar = nullptr;
                try {
                    sca_uchar = new HDF5CFByte(var->getNewName(), var->getFullPath(), filename);
                    sca_uchar->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_uchar);
                }
                catch (...) {
                    delete sca_uchar;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFByte.");
                }
                d4_grp->add_var_nocopy(sca_uchar);
            }
                break;
            case H5CHAR: {
                HDF5CFInt8 * sca_char = nullptr;
                try {
                    sca_char = new HDF5CFInt8(var->getNewName(), var->getFullPath(), filename);
                    sca_char->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_char);
                }
                catch (...) {
                    delete sca_char;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFByte.");
                }
                d4_grp->add_var_nocopy(sca_char);
            }
                break;

            case H5INT16: {
                HDF5CFInt16 * sca_int16 = nullptr;
                try {
                    sca_int16 = new HDF5CFInt16(var->getNewName(), var->getFullPath(), filename);
                    sca_int16->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_int16);
 
                }
                catch (...) {
                    delete sca_int16;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFInt16.");
                }
                d4_grp->add_var_nocopy(sca_int16);
            }
                break;
            case H5UINT16: {
                HDF5CFUInt16 * sca_uint16 = nullptr;
                try {
                    sca_uint16 = new HDF5CFUInt16(var->getNewName(), var->getFullPath(), filename);
                    sca_uint16->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_uint16);
                }
                catch (...) {
                    delete sca_uint16;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFUInt16.");
                }
                d4_grp->add_var_nocopy(sca_uint16);
            }
                break;
            case H5INT32: {
                HDF5CFInt32 * sca_int32 = nullptr;
                try {
                    sca_int32 = new HDF5CFInt32(var->getNewName(), var->getFullPath(), filename);
                    sca_int32->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_int32);
                }
                catch (...) {
                    delete sca_int32;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFInt32.");
                }
                d4_grp->add_var_nocopy(sca_int32);
            }
                break;
            case H5UINT32: {
                HDF5CFUInt32 * sca_uint32 = nullptr;
                try {
                    sca_uint32 = new HDF5CFUInt32(var->getNewName(), var->getFullPath(), filename);
                    sca_uint32->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_uint32);
                }
                catch (...) {
                    delete sca_uint32;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFUInt32.");
                }
                d4_grp->add_var_nocopy(sca_uint32);
            }
                break;
            case H5INT64: {
                HDF5CFInt64 * sca_int64 = nullptr;
                try {
                    sca_int64 = new HDF5CFInt64(var->getNewName(), var->getFullPath(), filename);
                    sca_int64->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_int64);
                }
                catch (...) {
                    delete sca_int64;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFInt64.");
                }
                d4_grp->add_var_nocopy(sca_int64);
            }
                break;
            case H5UINT64: {
                HDF5CFUInt64 * sca_uint64 = nullptr;
                try {
                    sca_uint64 = new HDF5CFUInt64(var->getNewName(), var->getFullPath(), filename);
                    sca_uint64->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_uint64);
                }
                catch (...) {
                    delete sca_uint64;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFUInt64.");
                }
                d4_grp->add_var_nocopy(sca_uint64);
            }
                break;
            case H5FLOAT32: {
                HDF5CFFloat32 * sca_float32 = nullptr;
                try {
                    sca_float32 = new HDF5CFFloat32(var->getNewName(), var->getFullPath(), filename);
                    sca_float32->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_float32);
                }
                catch (...) {
                    delete sca_float32;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFFloat32.");
                }
                d4_grp->add_var_nocopy(sca_float32);
            }
                break;
            case H5FLOAT64: {
                HDF5CFFloat64 * sca_float64 = nullptr;
                try {
                    sca_float64 = new HDF5CFFloat64(var->getNewName(), var->getFullPath(), filename);
                    sca_float64->set_is_dap4(true);
                    map_cfh5_var_attrs_to_dap4(var,sca_float64);
                }
                catch (...) {
                    delete sca_float64;
                    throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFFloat64.");
                }
                d4_grp->add_var_nocopy(sca_float64);
            }
                break;
            default:
                throw InternalErr(__FILE__, __LINE__, "unsupported data type.");
            }
        }
    }

    else {

        BaseType *bt = nullptr;

        switch (var->getType()) {
#define HANDLE_CASE(tid,type)                                  \
            case tid:                                           \
                bt = new (type)(var->getNewName(),var->getFullPath()); \
            break;
        HANDLE_CASE(H5FLOAT32, HDF5CFFloat32)
        HANDLE_CASE(H5FLOAT64, HDF5CFFloat64)
        HANDLE_CASE(H5CHAR, HDF5CFInt8)
        HANDLE_CASE(H5UCHAR, HDF5CFByte)
        HANDLE_CASE(H5INT16, HDF5CFInt16)
        HANDLE_CASE(H5UINT16, HDF5CFUInt16)
        HANDLE_CASE(H5INT32, HDF5CFInt32)
        HANDLE_CASE(H5UINT32, HDF5CFUInt32)
        HANDLE_CASE(H5INT64, HDF5CFInt64)
        HANDLE_CASE(H5UINT64, HDF5CFUInt64)
        HANDLE_CASE(H5FSTRING, Str)
        HANDLE_CASE(H5VSTRING, Str)
        default:
            throw InternalErr(__FILE__, __LINE__, "unsupported data type.");
#undef HANDLE_CASE
        }

        vector<size_t> dimsizes;
        dimsizes.resize(var->getRank());
        for (int i = 0; i < var->getRank(); i++)
            dimsizes[i] = (dims[i])->getSize();

        HDF5CFArray *ar = nullptr;
        try {
            ar = new HDF5CFArray(var->getRank(), file_id, filename, var->getType(), dimsizes, var->getFullPath(),
                var->getTotalElems(), CV_UNSUPPORTED, false, var->getCompRatio(), true,var->getNewName(), bt);
        }
        catch (...) {
            delete bt;
            throw InternalErr(__FILE__, __LINE__, "Cannot allocate the HDF5CFStr.");
        }

        for (auto it_d = dims.begin(); it_d != dims.end(); ++it_d) {
            if ("" == (*it_d)->getNewName())
                ar->append_dim((int)((*it_d)->getSize()));
            else
                ar->append_dim((int)((*it_d)->getSize()), (*it_d)->getNewName());
        }

        delete bt;
        ar->set_is_dap4(true);
        BaseType* d4_var=ar->h5cfdims_transform_to_dap4(d4_grp);
        map_cfh5_var_attrs_to_dap4(var,d4_var);
        d4_grp->add_var_nocopy(d4_var);
        delete ar;

    }

    return;
}

/**
 * @brief Transfer string attributes to a DAP2 AttrTable
 *
 * For an HDF5 string attribute stored in a HDF5CF::Attribute, transfer the
 * value(s) of that attribute to a DAP2 AttrTable object. This function
 * handles both single and multiple attribute value cases. It should
 * only be called for string attributes. Except as noted below, the
 * attribute values are escaped so that non-printable ASCII characters
 * are represented by a backslash and an octal code (e.g. \005).
 *
 * @note If the given HDF5CF::Attribute uses UTF-8, it will still be
 * escaped as if it were an ASCII string (or used iso-8859-1 encoding)
 * _unless_ the BES key H5.EscapeUTF8Attr is false (default is true).
 *
 * @note Attributes named 'origname' or 'fullnamepath' are never escaped.
 *
 * @param at Put the string attribute in this DAP2 AttrTable
 * @param attr The source attribute information
 */
void gen_dap_str_attr(AttrTable *at, const HDF5CF::Attribute *attr)
{
    BESDEBUG("h5", "Coming to gen_dap_str_attr()  " << endl);
    const vector<size_t>& strsize = attr->getStrSize();

    unsigned int temp_start_pos = 0;
    for (unsigned int loc = 0; loc < attr->getCount(); loc++) {
        if (strsize[loc] != 0) {
            string tempstring(attr->getValue().begin() + temp_start_pos, attr->getValue().begin() + temp_start_pos + strsize[loc]);
            temp_start_pos += strsize[loc];

            // If the string size is longer than the current netCDF JAVA
            // string and the "EnableDropLongString" key is turned on,
            // No string is generated.
            //
            // The above statement is no longer true. The netCDF Java can handle long string
            // attributes. The long string can be kept, and I do think the
            // performance penalty should be small. KY 2018-02-26
            //
            // Here is the logic to determine if the attribute value should be escaped.
            // Attributes named 'origname' or 'fullnamepath' are never escaped. Attributes
            // with values that use the UTF-8 character set _are_ encoded unless the
            // HD.EscapeUTF8Attr is set to false. If the attribute values use ASCII
            // (i.e., attr->getCsetType() is true), they are always escaped. jhrg 3/9/22
            if ((attr->getNewName() != "origname") && (attr->getNewName() != "fullnamepath")
                && (HDF5RequestHandler::get_escape_utf8_attr() || (true == attr->getCsetType()))) {
                tempstring = HDF5CFDAPUtil::escattr(tempstring);
            }
            at->append_attr(attr->getNewName(), "String", tempstring);
        }
    }
}

//#if 0
// This function adds the 1-D horizontal coordinate variables as well as the dummy projection variable to the grid.
// Note: Since we don't add these artificial CF variables to our main engineering at HDFEOS5CF.cc, the information
// to handle DAS won't pass to DDS by the file pointer, we need to re-call the routines to check projection
// and dimension. The time to retrieve this information is trivial compared with the whole translation.
void add_cf_grid_cvs(DDS & dds, EOS5GridPCType cv_proj_code, float cv_point_lower, float cv_point_upper,
    float cv_point_left, float cv_point_right, const vector<HDF5CF::Dimension*>& dims)
{

    //1. Check the projection information: we first just handled the sinusoidal projection. 
    // We also add the LAMAZ and PS support. These 1-D variables are the same as the sinusoidal one.
    if (HE5_GCTP_SNSOID == cv_proj_code || HE5_GCTP_LAMAZ == cv_proj_code || HE5_GCTP_PS == cv_proj_code) {

        //2. Obtain the dimension information from latitude and longitude(fieldtype =1 or fieldtype =2)
        string dim0name = dims[0]->getNewName();
        auto dim0size = (int)(dims[0]->getSize());
        string dim1name = dims[1]->getNewName();
        auto dim1size = (int)(dims[1]->getSize());

        //3. Add the 1-D CV variables and the dummy projection variable
        BaseType *bt_dim0 = nullptr;
        BaseType *bt_dim1 = nullptr;

        HDF5CFGeoCF1D * ar_dim0 = nullptr;
        HDF5CFGeoCF1D * ar_dim1 = nullptr;

        try {

            bt_dim0 = new (HDF5CFFloat64)(dim0name, dim0name);
            bt_dim1 = new (HDF5CFFloat64)(dim1name, dim1name);

            // Note ar_dim0 is y, ar_dim1 is x.
            ar_dim0 = new HDF5CFGeoCF1D(HE5_GCTP_SNSOID, cv_point_upper, cv_point_lower, dim0size, dim0name, bt_dim0);
            ar_dim0->append_dim(dim0size, dim0name);

            ar_dim1 = new HDF5CFGeoCF1D(HE5_GCTP_SNSOID, cv_point_left, cv_point_right, dim1size, dim1name, bt_dim1);
            ar_dim1->append_dim(dim1size, dim1name);
            dds.add_var(ar_dim0);
            dds.add_var(ar_dim1);

        }
        catch (...) {
            if (bt_dim0) delete bt_dim0;
            if (bt_dim1) delete bt_dim1;
            if (ar_dim0) delete ar_dim0;
            if (ar_dim1) delete ar_dim1;
            throw InternalErr(__FILE__, __LINE__, "Unable to allocate the HDFEOS2GeoCF1D instance.");
        }

        if (bt_dim0) delete bt_dim0;
        if (bt_dim1) delete bt_dim1;
        if (ar_dim0) delete ar_dim0;
        if (ar_dim1) delete ar_dim1;

    }
}

// This function adds the grid mapping variables.
void add_cf_grid_mapinfo_var(DDS & dds, const EOS5GridPCType grid_proj_code, const unsigned short g_suffix)
{

    //Add the dummy projection variable. The attributes of this variable can be used to store the grid mapping info.
    // To handle multi-grid cases, we need to add suffixes to distinguish them.
    string cf_projection_base = "eos_cf_projection";

    HDF5CFGeoCFProj * dummy_proj_cf = nullptr;
    if(HE5_GCTP_SNSOID == grid_proj_code)  {
        // AFAWK, one grid_mapping variable is necessary for multi-grids. So we just leave one grid here.
        if(g_suffix == 1) {
            dummy_proj_cf = new HDF5CFGeoCFProj(cf_projection_base, cf_projection_base);
            dds.add_var(dummy_proj_cf);
        }
    }
    else {
        stringstream t_suffix_ss;
        t_suffix_ss << g_suffix;
        string cf_projection_name = cf_projection_base + "_" + t_suffix_ss.str();
        dummy_proj_cf = new HDF5CFGeoCFProj(cf_projection_name, cf_projection_name);
        dds.add_var(dummy_proj_cf);
    }
    if (dummy_proj_cf) delete dummy_proj_cf;

}

// This function adds 1D grid mapping CF attributes to CV and data variables.
#if 0
void add_cf_grid_cv_attrs(DAS & das, const vector<HDF5CF::Var*>& vars, EOS5GridPCType cv_proj_code,
    float /*cv_point_lower*/, float /*cv_point_upper*/, float /*cv_point_left*/, float /*cv_point_right*/,
    const vector<HDF5CF::Dimension*>& dims,const vector<double> &eos5_proj_params,const unsigned short g_suffix)
#endif
void add_cf_grid_cv_attrs(DAS & das, const vector<HDF5CF::Var*>& vars, EOS5GridPCType cv_proj_code,
    const vector<HDF5CF::Dimension*>& dims,const vector<double> &eos5_proj_params,const unsigned short g_suffix)
{


    //1. Check the projection information, now, we handle sinusoidal,PS and LAMAZ projections.
    if (HE5_GCTP_SNSOID == cv_proj_code || HE5_GCTP_PS == cv_proj_code || HE5_GCTP_LAMAZ== cv_proj_code) {

        string dim0name = (dims[0])->getNewName();
        auto dim0size = (int)(dims[0]->getSize());
        string dim1name = (dims[1])->getNewName();
        auto dim1size = (int)(dims[1]->getSize());

        //2. Add 1D CF attributes to the 1-D CV variables and the dummy grid_mapping variable
        AttrTable *at = das.get_table(dim0name);
        if (!at) 
            at = das.add_table(dim0name, new AttrTable);
        at->append_attr("standard_name", "String", "projection_y_coordinate");

        string long_name = "y coordinate of projection ";
        at->append_attr("long_name", "String", long_name);

        // Change this to meter.
        at->append_attr("units", "string", "meter");

        at->append_attr("_CoordinateAxisType", "string", "GeoY");

        at = das.get_table(dim1name);
        if (!at) at = das.add_table(dim1name, new AttrTable);

        at->append_attr("standard_name", "String", "projection_x_coordinate");

        long_name = "x coordinate of projection ";
        at->append_attr("long_name", "String", long_name);

        // change this to meter.
        at->append_attr("units", "string", "meter");
        
        // This is for CDM conventions. Adding doesn't do harm. Same as GeoY.
        at->append_attr("_CoordinateAxisType", "string", "GeoX");

        // Add the attributes for the dummy grid_mapping variable.
        string cf_projection_base = "eos_cf_projection";
        string cf_projection;
        if(HE5_GCTP_SNSOID == cv_proj_code)
            cf_projection = cf_projection_base;
        else {
            stringstream t_suffix_ss;
            t_suffix_ss << g_suffix;
            cf_projection = cf_projection_base + "_" + t_suffix_ss.str();
        }
	add_cf_projection_attrs(das,cv_proj_code,eos5_proj_params,cf_projection);

        // Fill in the data fields that contains the dim0name and dim1name dimensions with the grid_mapping
        // We only apply to >=2D data fields.
        add_cf_grid_mapping_attr(das, vars, cf_projection, dim0name, dim0size, dim1name, dim1size);
    }

}

// Add CF projection attribute

void add_cf_projection_attrs(DAS &das,EOS5GridPCType cv_proj_code,const vector<double> &eos5_proj_params,const string& cf_projection) {

    AttrTable* at = das.get_table(cf_projection);
    if (!at) {// Only append attributes when the table is created.
        at = das.add_table(cf_projection, new AttrTable);

        if (HE5_GCTP_SNSOID == cv_proj_code) {
            at->append_attr("grid_mapping_name", "String", "sinusoidal");
            at->append_attr("longitude_of_central_meridian", "Float64", "0.0");
            at->append_attr("earth_radius", "Float64", "6371007.181");
            at->append_attr("_CoordinateAxisTypes", "string", "GeoX GeoY");
        }
        else if (HE5_GCTP_PS == cv_proj_code) {

            // The following information is added according to the HDF-EOS5 user's guide and
            // CF 1.7 grid_mapping requirement.

            // Longitude down below pole of map
            double vert_lon_pole =  HE5_EHconvAng(eos5_proj_params[4],HE5_HDFE_DMS_DEG);

            // Latitude of true scale
            double lat_true_scale = HE5_EHconvAng(eos5_proj_params[5],HE5_HDFE_DMS_DEG);

            // False easting
            double fe = eos5_proj_params[6];

            // False northing 
            double fn = eos5_proj_params[7];

            at->append_attr("grid_mapping_name", "String", "polar_stereographic");

            ostringstream s_vert_lon_pole;
            s_vert_lon_pole << vert_lon_pole;

            // I did this map is based on my best understanding. I cannot be certain about south pole. KY
            // CF: straight_vertical_longitude_from_pole
            at->append_attr("straight_vertical_longitude_from_pole", "Float64", s_vert_lon_pole.str());
            ostringstream s_lat_true_scale;
            s_lat_true_scale << lat_true_scale;

            at->append_attr("standard_parallel", "Float64", s_lat_true_scale.str());

            if(fe == 0.0) 
                at->append_attr("false_easting","Float64","0.0");
            else { 
                ostringstream s_fe;
                s_fe << fe;
                at->append_attr("false_easting","Float64",s_fe.str());
            }


            if(fn == 0.0) 
                at->append_attr("false_northing","Float64","0.0");
            else { 
                ostringstream s_fn;
                s_fn << fn;
                at->append_attr("false_northing","Float64",s_fn.str());
            }

            
            if(lat_true_scale >0) 
                at->append_attr("latitude_of_projection_origin","Float64","+90.0");
            else 
                at->append_attr("latitude_of_projection_origin","Float64","-90.0");


            at->append_attr("_CoordinateAxisTypes", "string", "GeoX GeoY");

            // From CF, PS has another parameter,
            // Either standard_parallel (EPSG 9829) or scale_factor_at_projection_origin (EPSG 9810)
            // I cannot find the corresponding parameter from the EOS5.

        }
        else if(HE5_GCTP_LAMAZ == cv_proj_code) {
            double lon_proj_origin = HE5_EHconvAng(eos5_proj_params[4],HE5_HDFE_DMS_DEG);
            double lat_proj_origin = HE5_EHconvAng(eos5_proj_params[5],HE5_HDFE_DMS_DEG);
            double fe = eos5_proj_params[6];
            double fn = eos5_proj_params[7];

            at->append_attr("grid_mapping_name", "String", "lambert_azimuthal_equal_area");

            ostringstream s_lon_proj_origin;
            s_lon_proj_origin << lon_proj_origin;
            at->append_attr("longitude_of_projection_origin", "Float64", s_lon_proj_origin.str());
            
            ostringstream s_lat_proj_origin;
            s_lat_proj_origin << lat_proj_origin;
 
            at->append_attr("latitude_of_projection_origin", "Float64", s_lat_proj_origin.str());


            if(fe == 0.0) 
                at->append_attr("false_easting","Float64","0.0");
            else { 
                ostringstream s_fe;
                s_fe << fe;
                at->append_attr("false_easting","Float64",s_fe.str());
            }


            if(fn == 0.0) 
                at->append_attr("false_northing","Float64","0.0");
            else { 
                ostringstream s_fn;
                s_fn << fn;
                at->append_attr("false_northing","Float64",s_fn.str());
            }

            at->append_attr("_CoordinateAxisTypes", "string", "GeoX GeoY");


        }
    }

}


// This function adds the 1-D cf grid projection mapping attribute to data variables
// it is called by the function add_cf_grid_attrs. 
void add_cf_grid_mapping_attr(DAS &das, const vector<HDF5CF::Var*>& vars, const string& cf_projection,
    const string & dim0name, hsize_t dim0size, const string &dim1name, hsize_t dim1size)
{

#if 0
    cerr<<"dim0name is "<<dim0name <<endl;
    cerr<<"dim1name is "<<dim1name <<endl;
    cerr<<"dim0size is "<<dim0size <<endl;
    cerr<<"dim1size is "<<dim1size <<endl;
#endif

    // Check >=2-D fields, check if they hold the dim0name,dim0size etc., yes, add the attribute cf_projection.
    for (auto it_v = vars.begin(); it_v != vars.end(); ++it_v) {

        if ((*it_v)->getRank() > 1) {
            bool has_dim0 = false;
            bool has_dim1 = false;
            const vector<HDF5CF::Dimension*>& dims = (*it_v)->getDimensions();
            for (auto j = dims.begin(); j != dims.end(); ++j) {
                if ((*j)->getNewName() == dim0name && (*j)->getSize() == dim0size)
                    has_dim0 = true;
                else if ((*j)->getNewName() == dim1name && (*j)->getSize() == dim1size) 
                    has_dim1 = true;

            }
            if (true == has_dim0 && true == has_dim1) {        // Need to add the grid_mapping attribute
                AttrTable *at = das.get_table((*it_v)->getNewName());
                if (!at) at = das.add_table((*it_v)->getNewName(), new AttrTable);

                // The dummy projection name is the value of the grid_mapping attribute
                at->append_attr("grid_mapping", "String", cf_projection);
            }
        }
    }
}
// Now this is specially for LAMAZ where the NSIDC EASE-Grid may have points off the earth. So
// The calculated lat/lon are set to number out of the normal range. The valid_range attributes
// will hopefully constrain the applications not to consider those points.
void add_ll_valid_range(AttrTable* at, bool is_lat) {
    if(true == is_lat) {
        at->append_attr("valid_min", "Float64","-90.0");
        at->append_attr("valid_max", "Float64","90.0");
    }
    else {
        at->append_attr("valid_min", "Float64","-180.0");
        at->append_attr("valid_max", "Float64","180.0");
    }
}

// This routine is for 64-bit DAP4 CF support: when var type is 64-bit integer.
// Note: the main part of DMR still comes from DDS and DAS.
bool need_attr_values_for_dap4(const HDF5CF::Var *var) {
    bool ret_value = false;
    if((HDF5RequestHandler::get_dmr_64bit_int()!=nullptr) && 
        (H5INT64 == var->getType() || H5UINT64 == var->getType()))
        ret_value = true;
    return ret_value;
}

// This routine is for 64-bit DAP4 CF support: map all attributes to DAP4 for 64-bit integers.
// Note: the main part of DMR still comes from DDS and DAS.
void map_cfh5_var_attrs_to_dap4_int64(const HDF5CF::Var *var,BaseType* d4_var) {

    for (auto it_ra = var->getAttributes().begin();
        it_ra != var->getAttributes().end(); ++it_ra) {
        // HDF5 Native Char maps to DAP INT16(DAP doesn't have the corresponding datatype), so needs to
        // obtain the mem datatype. Keep this in DAP4 mapping.
        size_t mem_dtype_size = ((*it_ra)->getBufSize()) / ((*it_ra)->getCount());
        H5DataType mem_dtype = HDF5CFDAPUtil::get_mem_dtype((*it_ra)->getType(), mem_dtype_size);

        string dap2_attrtype = HDF5CFDAPUtil::print_type(mem_dtype);
        D4AttributeType dap4_attrtype = HDF5CFDAPUtil::daptype_strrep_to_dap4_attrtype(dap2_attrtype);
        auto d4_attr = new D4Attribute((*it_ra)->getNewName(),dap4_attrtype);
        if(dap4_attrtype == attr_str_c) {
            if("coordinates" == (*it_ra)->getNewName()) {
                bool chg_coor_value = false;
                if((true == HDF5RequestHandler::get_enable_coord_attr_add_path()) 
                   &&(true == var->getCoorAttrAddPath()))
                    chg_coor_value = true;
                string tempstring;
                handle_coor_attr_for_int64_var((*it_ra),var->getFullPath(),tempstring,chg_coor_value);
                d4_attr->add_value(tempstring);
            }
            else {
                const vector<size_t>& strsize = (*it_ra)->getStrSize();
                unsigned int temp_start_pos = 0;
                for (unsigned int loc = 0; loc < (*it_ra)->getCount(); loc++) {
                    if (strsize[loc] != 0) {
                        string tempstring((*it_ra)->getValue().begin() + temp_start_pos,
                                      (*it_ra)->getValue().begin() + temp_start_pos + strsize[loc]);
                        temp_start_pos += strsize[loc];
                        //The below if is not necessary since the "origname" and "fullnamepath" are not added.KY 2020-02-24
#if 0
                        //if (((*it_ra)->getNewName() != "origname") && ((*it_ra)->getNewName() != "fullnamepath")) 
#endif
                        tempstring = HDF5CFDAPUtil::escattr(tempstring);
                        d4_attr->add_value(tempstring);
                    }
                }
            }

        }
        else {
            for (unsigned int loc = 0; loc < (*it_ra)->getCount(); loc++) {
                string print_rep = HDF5CFDAPUtil::print_attr(mem_dtype, loc, (void*) &((*it_ra)->getValue()[0]));
                d4_attr->add_value(print_rep);
            }
        }
        d4_var->attributes()->add_attribute_nocopy(d4_attr);
    }
    // Here we add the "origname" and "fullnamepath" attributes since they are crucial to DMRPP generation.
    auto d4_attr = new D4Attribute("origname",attr_str_c);
    d4_attr->add_value(var->getName());
    d4_var->attributes()->add_attribute_nocopy(d4_attr);
    d4_attr = new D4Attribute("fullnamepath",attr_str_c);
    d4_attr->add_value(var->getFullPath());
    d4_var->attributes()->add_attribute_nocopy(d4_attr);
}

// A helper function for 64-bit DAP4 CF support
// Note: the main part of DMR still comes from DDS and DAS.
void check_update_int64_attr(const string & obj_name, const HDF5CF::Attribute * attr) {
    if(attr->getType() == H5INT64 || attr->getType() == H5UINT64) { 

        DMR * dmr = HDF5RequestHandler::get_dmr_64bit_int();
        if(dmr != nullptr) {
            string dap2_attrtype = HDF5CFDAPUtil::print_type(attr->getType());
            D4AttributeType dap4_attrtype = HDF5CFDAPUtil::daptype_strrep_to_dap4_attrtype(dap2_attrtype);
            D4Attribute *d4_attr = new D4Attribute(attr->getNewName(),dap4_attrtype);
            for (unsigned int loc = 0; loc < attr->getCount(); loc++) {
                string print_rep = HDF5CFDAPUtil::print_attr(attr->getType(), loc, (void*) &(attr->getValue()[0]));
                d4_attr->add_value(print_rep);
            }
            D4Group * root_grp = dmr->root();
            D4Attribute *d4_hg_container; 
            if(root_grp->attributes()->empty() == true){
#if 0
            //D4Attribute *d4_hg_container = root_grp->attributes()->find("HDF5_GLOBAL");
            //if(d4_hg_container == nullptr) {
#endif
                d4_hg_container = new D4Attribute;
                d4_hg_container->set_name("HDF5_GLOBAL_integer_64");
                d4_hg_container->set_type(attr_container_c);
                root_grp->attributes()->add_attribute_nocopy(d4_hg_container);
#if 0
                //root_grp->attributes()->add_attribute(d4_hg_container);
#endif
            }
            //else 
            d4_hg_container = root_grp->attributes()->get("HDF5_GLOBAL_integer_64");
            if(obj_name != "") {
                string test_obj_name = "HDF5_GLOBAL_integer_64."+obj_name;
#if 0
                //D4Attribute *d4_container = root_grp->attributes()->find(obj_name);
                //D4Attribute *d4_container = root_grp->attributes()->get(obj_name);
#endif
                D4Attribute *d4_container = root_grp->attributes()->get(test_obj_name);
                // ISSUES need to search the attributes 
                //
#if 0
                //D4Attribute *d4_container = d4_hg_container->attributes()->find(obj_name);
#endif
                if(d4_container == nullptr) {
                    d4_container = new D4Attribute;
                    d4_container->set_name(obj_name);
                    d4_container->set_type(attr_container_c);

#if 0
                    //if(d4_hg_container->attributes()->empty()==true)
                    //    cerr<<"global container is empty"<<endl;
                    //d4_hg_container->attributes()->add_attribute_nocopy(d4_container);
                    //cerr<<"end of d4_container "<<endl;
#endif
                }
                d4_container->attributes()->add_attribute_nocopy(d4_attr);
#if 0
                //root_grp->attributes()->add_attribute_nocopy(d4_container);
#endif
//#if 0
                if(d4_hg_container->attributes()->get(obj_name)==nullptr)
                    d4_hg_container->attributes()->add_attribute_nocopy(d4_container);
//#endif
            }
            else 
                d4_hg_container->attributes()->add_attribute_nocopy(d4_attr);
        }
    }
}

// Another helper function for 64-bit DAP4 CF support
// Note: the main part of DMR still comes from DDS and DAS.
void handle_coor_attr_for_int64_var(const HDF5CF::Attribute *attr,const string &var_path,string &tempstring,bool chg_coor_value) {

    string tempstring2(attr->getValue().begin(),attr->getValue().end()); 
    if(true == chg_coor_value) {
        char sep=' ';
        vector<string>cvalue_vec;
        HDF5CFUtil::Split_helper(cvalue_vec,tempstring2,sep);
        for (int i = 0; i<cvalue_vec.size();i++) {
            HDF5CFUtil::cha_co(cvalue_vec[i],var_path);
            string t_str = get_cf_string(cvalue_vec[i]);
            if(i == 0) 
                tempstring = t_str;
            else 
                tempstring += sep+t_str;
        }
    }
    else 
        tempstring = tempstring2;

}

// This routine is for direct mapping from CF to DAP4. We build DMR not from DDS and DAS. 
// Hopefully this will be eventually used to build DMR. 
void map_cfh5_var_attrs_to_dap4(const HDF5CF::Var *var,BaseType* d4_var) {

    for (auto it_ra = var->getAttributes().begin();
        it_ra != var->getAttributes().end(); ++it_ra) {
     
        D4Attribute *d4_attr = gen_dap4_attr((*it_ra));
        d4_var->attributes()->add_attribute_nocopy(d4_attr);
    }
}

// This routine is for direct mapping from CF to DAP4. We build DMR not from DDS and DAS. 
void map_cfh5_grp_attr_to_dap4(libdap::D4Group *d4_grp,const HDF5CF::Attribute *attr) {

    D4Attribute *d4_attr = gen_dap4_attr(attr);
    d4_grp->attributes()->add_attribute_nocopy(d4_attr);

}

// This routine is for direct mapping from CF to DAP4. We build DMR not from DDS and DAS. 
void map_cfh5_attr_container_to_dap4(libdap::D4Attribute *d4_con,const HDF5CF::Attribute *attr) {

    D4Attribute *d4_attr = gen_dap4_attr(attr);
    d4_con->attributes()->add_attribute_nocopy(d4_attr);

}

// Helper function to generate a DAP4 attribute.
D4Attribute *gen_dap4_attr(const HDF5CF::Attribute *attr) {

    D4AttributeType dap4_attrtype = HDF5CFDAPUtil::print_type_dap4(attr->getType());
    auto d4_attr = new D4Attribute(attr->getNewName(),dap4_attrtype);
    if(dap4_attrtype == attr_str_c) {
            
        const vector<size_t>& strsize = attr->getStrSize();
#if 0
        if(strsize.size()  == 0)
            cerr << "vector string size is 0" << endl;
        for(int i = 0; i<strsize.size(); i++)
            cerr << "attr size  is "<<strsize[i] << endl;
#endif
        unsigned int temp_start_pos = 0;
        for (unsigned int loc = 0; loc < attr->getCount(); loc++) {
            if (strsize[loc] != 0) {
                string tempstring(attr->getValue().begin() + temp_start_pos,
                                  attr->getValue().begin() + temp_start_pos + strsize[loc]);
                temp_start_pos += strsize[loc];
                if ((attr->getNewName() != "origname") && (attr->getNewName() != "fullnamepath")
                    && (HDF5RequestHandler::get_escape_utf8_attr() || (true == attr->getCsetType()))) {
                    tempstring = HDF5CFDAPUtil::escattr(tempstring);
                }
                d4_attr->add_value(tempstring);
            }
        }
    }
    else {
#if 0
        cerr << "not a string type " << endl;
#endif
        for (unsigned int loc = 0; loc < attr->getCount(); loc++) {
            string print_rep = HDF5CFDAPUtil::print_attr(attr->getType(), loc, (void*) &(attr->getValue()[0]));
            d4_attr->add_value(print_rep);
        }
    }

    return d4_attr;
}

// Direct CF to DAP4, add CF grid_mapping attributes of the projection variable to DAP4.  
// we support sinusodial, polar stereographic and lambert azimuthal equal-area(LAMAZ) projections.
void add_gm_oneproj_var_dap4_attrs(BaseType *var,EOS5GridPCType cv_proj_code,const vector<double> &eos5_proj_params) {

    if (HE5_GCTP_SNSOID == cv_proj_code) {

        add_var_dap4_attr(var,"grid_mapping_name",attr_str_c,"sinusoidal"); 
        add_var_dap4_attr(var,"longitude_of_central_meridian",attr_float64_c,"0.0"); 
        add_var_dap4_attr(var,"earth_radius", attr_float64_c, "6371007.181");
        add_var_dap4_attr(var,"_CoordinateAxisTypes", attr_str_c, "GeoX GeoY");

    }
    else if (HE5_GCTP_PS == cv_proj_code) {

        // The following information is added according to the HDF-EOS5 user's guide and
        // CF 1.7 grid_mapping requirement.

        // Longitude down below pole of map
        double vert_lon_pole =  HE5_EHconvAng(eos5_proj_params[4],HE5_HDFE_DMS_DEG);

        // Latitude of true scale
        double lat_true_scale = HE5_EHconvAng(eos5_proj_params[5],HE5_HDFE_DMS_DEG);

        // False easting
        double fe = eos5_proj_params[6];

        // False northing 
        double fn = eos5_proj_params[7];

        add_var_dap4_attr(var,"grid_mapping_name",attr_str_c,"polar_stereographic"); 

        ostringstream s_vert_lon_pole;
        s_vert_lon_pole << vert_lon_pole;

        // I did this map is based on my best understanding. I cannot be certain about south pole. KY
        // CF: straight_vertical_longitude_from_pole
#if 0
        //at->append_attr("straight_vertical_longitude_from_pole", "Float64", s_vert_lon_pole.str());
#endif
        add_var_dap4_attr(var,"straight_vertical_longitude_from_pole", attr_float64_c, s_vert_lon_pole.str());

        ostringstream s_lat_true_scale;
        s_lat_true_scale << lat_true_scale;
        add_var_dap4_attr(var,"standard_parallel", attr_float64_c, s_lat_true_scale.str());

        if(fe == 0.0) 
            add_var_dap4_attr(var,"false_easting",attr_float64_c,"0.0"); 
        else { 
            ostringstream s_fe;
            s_fe << fe;
            add_var_dap4_attr(var,"false_easting",attr_float64_c,s_fe.str());
        }

        if(fn == 0.0) 
            add_var_dap4_attr(var,"false_northing",attr_float64_c,"0.0");
        else { 
            ostringstream s_fn;
            s_fn << fn;
            add_var_dap4_attr(var,"false_northing",attr_float64_c,s_fn.str());
        }
        
        if(lat_true_scale >0) 
            add_var_dap4_attr(var,"latitude_of_projection_origin",attr_float64_c,"+90.0");
        else 
            add_var_dap4_attr(var, "latitude_of_projection_origin",attr_float64_c,"-90.0");

        add_var_dap4_attr(var, "_CoordinateAxisTypes", attr_str_c, "GeoX GeoY");

        // From CF, PS has another parameter,
        // Either standard_parallel (EPSG 9829) or scale_factor_at_projection_origin (EPSG 9810)
        // I cannot find the corresponding parameter from the EOS5.

    }
    else if(HE5_GCTP_LAMAZ == cv_proj_code) {

        double lon_proj_origin = HE5_EHconvAng(eos5_proj_params[4],HE5_HDFE_DMS_DEG);
        double lat_proj_origin = HE5_EHconvAng(eos5_proj_params[5],HE5_HDFE_DMS_DEG);
        double fe = eos5_proj_params[6];
        double fn = eos5_proj_params[7];

        add_var_dap4_attr(var,"grid_mapping_name", attr_str_c, "lambert_azimuthal_equal_area");

        ostringstream s_lon_proj_origin;
        s_lon_proj_origin << lon_proj_origin;
        add_var_dap4_attr(var,"longitude_of_projection_origin", attr_float64_c, s_lon_proj_origin.str());
        
        ostringstream s_lat_proj_origin;
        s_lat_proj_origin << lat_proj_origin;

        add_var_dap4_attr(var,"latitude_of_projection_origin", attr_float64_c, s_lat_proj_origin.str());

        if(fe == 0.0) 
            add_var_dap4_attr(var,"false_easting",attr_float64_c,"0.0");
        else { 
            ostringstream s_fe;
            s_fe << fe;
            add_var_dap4_attr(var,"false_easting",attr_float64_c,s_fe.str());
        }

        if(fn == 0.0) 
            add_var_dap4_attr(var,"false_northing",attr_float64_c,"0.0");
        else { 
            ostringstream s_fn;
            s_fn << fn;
            add_var_dap4_attr(var,"false_northing",attr_float64_c,s_fn.str());
        }

        add_var_dap4_attr(var,"_CoordinateAxisTypes", attr_str_c, "GeoX GeoY");
    }

}

// Direct CF to DAP4, add the CF "grid_mapping_name" attribute to every variable that uses the grid.
void add_cf_grid_cv_dap4_attrs(D4Group *d4_root, const string& cf_projection,      
                               const vector<HDF5CF::Dimension*>& dims, const vector<string> & cvar_name)                    
{
    // dims are dimensions for a grid. It is always 2-D for the projections we support.t
    string dim0name = (dims[0])->getNewName();
    hsize_t dim0size = dims[0]->getSize();
    string dim1name = (dims[1])->getNewName();
    hsize_t dim1size = dims[1]->getSize();

    // We only add the attribute to the variables that match the grid dimensions.
    Constructor::Vars_iter vi = d4_root->var_begin();
    Constructor::Vars_iter ve = d4_root->var_end();
    for (; vi != ve; vi++) {
        // Should not add grid_mapping info for the coordinate variables. 
        if((*vi)->is_vector_type() && (cvar_name.end() == find(cvar_name.begin(), cvar_name.end(),(*vi)->name()))) {
            auto t_a = dynamic_cast<Array*>(*vi);
            if(t_a->dimensions() >1) {
                Array::Dim_iter dim_i = t_a->dim_begin();
                Array::Dim_iter dim_e = t_a->dim_end();
                bool has_dim0 = false;
                bool has_dim1 = false;
                for(;dim_i !=dim_e;dim_i++) {
                    if((*dim_i).name == dim0name && (*dim_i).size == dim0size)
                        has_dim0 = true;
                    else if((*dim_i).name == dim1name && (*dim_i).size == dim1size)                              
                        has_dim1 = true;
                }

                if(true == has_dim0 && true == has_dim1) 
                    add_var_dap4_attr((*vi),"grid_mapping",attr_str_c,cf_projection);
            }
        }
    }
}


// Direct CF to DAP4, add special CF grid_mapping variable to DAP4.  
// These variables are dimension variables.
void add_gm_spcvs(libdap::D4Group *d4_root, EOS5GridPCType cv_proj_code, float cv_point_lower, float cv_point_upper,
                  float cv_point_left, float cv_point_right, const std::vector<HDF5CF::Dimension*>& dims) {

    //1. Check the projection information: we first just handled the sinusoidal projection. 
    // We also add the LAMAZ and PS support. These 1-D varaibles are the same as the sinusoidal one.
    if (HE5_GCTP_SNSOID == cv_proj_code || HE5_GCTP_LAMAZ == cv_proj_code || HE5_GCTP_PS == cv_proj_code) {

        //2. Obtain the dimension information from latitude and longitude(fieldtype =1 or fieldtype =2)
        string dim0name = dims[0]->getNewName();
        auto dim0size = (int)(dims[0]->getSize());
        string dim1name = dims[1]->getNewName();
        auto dim1size = (int)(dims[1]->getSize());

        //3. Add the 1-D CV variables and the dummy projection variable
        BaseType *bt_dim0 = nullptr;
        BaseType *bt_dim1 = nullptr;

        HDF5CFGeoCF1D * ar_dim0 = nullptr;
        HDF5CFGeoCF1D * ar_dim1 = nullptr;

        try {

            bt_dim0 = new (HDF5CFFloat64)(dim0name, dim0name);
            bt_dim1 = new (HDF5CFFloat64)(dim1name, dim1name);

            // Note ar_dim0 is y, ar_dim1 is x.
            ar_dim0 = new HDF5CFGeoCF1D(HE5_GCTP_SNSOID, cv_point_upper, cv_point_lower, dim0size, dim0name, bt_dim0);
            ar_dim0->append_dim(dim0size, dim0name);

            ar_dim0->set_is_dap4(true);

            add_gm_spcvs_attrs(ar_dim0,true);

            ar_dim1 = new HDF5CFGeoCF1D(HE5_GCTP_SNSOID, cv_point_left, cv_point_right, dim1size, dim1name, bt_dim1);
            ar_dim1->append_dim(dim1size, dim1name);

            ar_dim1->set_is_dap4(true);

            add_gm_spcvs_attrs(ar_dim1,false);

            d4_root->add_var(ar_dim0);
            d4_root->add_var(ar_dim1);

        }
        catch (...) {
            if (bt_dim0) delete bt_dim0;
            if (bt_dim1) delete bt_dim1;
            if (ar_dim0) delete ar_dim0;
            if (ar_dim1) delete ar_dim1;
            throw InternalErr(__FILE__, __LINE__, "Unable to allocate the HDFEOS2GeoCF1D instance.");
        }

        if (bt_dim0) delete bt_dim0;
        if (bt_dim1) delete bt_dim1;
        if (ar_dim0) delete ar_dim0;
        if (ar_dim1) delete ar_dim1;
    }
}

// Direct CF to DAP4, 
// add CF grid_mapping $attributes for the special dimension variables.  
void add_gm_spcvs_attrs(libdap::BaseType *var,const bool is_dim0) {

    string standard_name;
    string long_name;
    string COORAxisTypes;

    if (true == is_dim0) {
        standard_name = "projection_y_coordinate";
        long_name = "y coordinate of projection ";
        COORAxisTypes = "GeoY";
    }
    else {
        standard_name = "projection_x_coordinate";
        long_name = "x coordinate of projection ";
        COORAxisTypes = "GeoX";
    }
       
    add_var_dap4_attr(var,"standard_name", attr_str_c, standard_name);
    add_var_dap4_attr(var,"long_name", attr_str_c, long_name);
    add_var_dap4_attr(var,"units", attr_str_c, "meter");
    add_var_dap4_attr(var,"_CoordinateAxisType", attr_str_c, COORAxisTypes);

}

// Direct CF to DAP4, helper function to DAP4 group  attributes.  
void add_grp_dap4_attr(D4Group *d4_grp,const string& attr_name, D4AttributeType attr_type, const string& attr_value){

    auto d4_attr = new D4Attribute(attr_name,attr_type);
    d4_attr->add_value(attr_value);
    d4_grp->attributes()->add_attribute_nocopy(d4_attr);

}
// Direct CF to DAP4, helper function to DAP4 variable  attributes.  
void add_var_dap4_attr(BaseType *var,const string& attr_name, D4AttributeType attr_type, const string& attr_value){

    auto d4_attr = new D4Attribute(attr_name,attr_type);
    d4_attr->add_value(attr_value);
    var->attributes()->add_attribute_nocopy(d4_attr);

}

// Add DAP4 coverage 
void add_dap4_coverage(libdap::D4Group* d4_root, const vector<string>& coord_var_names, bool is_coard) {

    // We need to construct the var name to Array map,using unordered_map for quick search.
    unordered_map<string, Array*> d4map_array_maps;

    // This vector holds all variables that can have coverage maps.
    vector<Array*> has_map_arrays;
  
    Constructor::Vars_iter vi = d4_root->var_begin();
    Constructor::Vars_iter ve = d4_root->var_end();

    for (; vi != ve; vi++) {

        BaseType *v = *vi;

        // Only Array can have maps.
        if (libdap::dods_array_c == v->type()) {

            auto t_a = static_cast<Array *>(*vi);

            // The maps are essentially coordinate variables.
            // We've sorted the coordinate variables already, so
            // just save them to the vector. 
            // Note: the coordinate variables we collect here are
            // based on the understanding of the handler. We will
            // watch if there are complicated cases down the road. KY 04-15-2022
            bool is_cv = false;
            for ( auto it_cv = coord_var_names.begin(); it_cv !=coord_var_names.end();++it_cv) {
                if ((*it_cv) == v->name()) {
                    is_cv = true;
                    d4map_array_maps.emplace(v->name(),t_a); 
                    break;
                }
            }

            // If this is not a map variable, it has a good chance to hold maps.
            if (is_cv == false) {
                has_map_arrays.emplace_back(t_a);
            }
        }
    }

    // loop through has_map_arrays to add the maps.
    if (is_coard) {// The grid case. 
        for ( auto it_hm = has_map_arrays.begin(); it_hm != has_map_arrays.end(); ++it_hm) {

            Array::Dim_iter dim_i = (*it_hm)->dim_begin();
            Array::Dim_iter dim_e = (*it_hm)->dim_end();
            for (; dim_i != dim_e; dim_i++) {

                // The dimension name is the same as a map name(A Grid case) 
                // Need to ensure the map array can be found.
                unordered_map<string, Array*>::const_iterator it_ma = d4map_array_maps.find(dim_i->name);
                if(it_ma != d4map_array_maps.end()) {
                    auto d4_map = new D4Map((it_ma->second)->FQN(), it_ma->second, *it_hm);
                    (*it_hm)->maps()->add_map(d4_map);
                }
            }
            // Need to set the has_map_arrays to 0 to avoid calling ~Array() when the vector goes out of loop.
            *it_hm = nullptr;
        }
    }
    else { // A Swath case, need to find coordinates and retrieve the values.

        for ( auto it_hm = has_map_arrays.begin(); it_hm != has_map_arrays.end(); ++it_hm) {

            // If we cannot find the "coordinates",then this var doesn't have a map.
            vector<string> coord_names;
            D4Attributes *d4_attrs = (*it_hm)->attributes();
            const D4Attribute *d4_attr = d4_attrs->find("coordinates");
            if (d4_attr != nullptr) {
                // For all the coordinates the CF option can handle,  
                // the attribute is a one-element string.
                if(d4_attr->type() == attr_str_c && d4_attr->num_values() == 1) {
                    string tempstring = d4_attr->value(0);
                    char sep=' ';
                    HDF5CFUtil::Split_helper(coord_names,tempstring,sep);
                }
            }

            // Search if these coordiates can be found in the coordinate variable list.
            for(auto it_c = coord_names.begin(); it_c != coord_names.end(); ++it_c) {

                unordered_map<string, Array*>::const_iterator it_ma = d4map_array_maps.find(*it_c);
                if(it_ma != d4map_array_maps.end()) {
                    auto d4_map = new D4Map((it_ma->second)->FQN(), it_ma->second, *it_hm);
                    (*it_hm)->maps()->add_map(d4_map);
                }

            }

            // Need to set the has_map_arrays to 0 to avoid calling ~Array() when the vector goes out of loop.
            *it_hm = nullptr;
        }
    }
    // We need to set the second element of the d4map_array_maps to 0 to avoid the ~Array() is called
    // when this map goes out of loop.
    for (auto it_dm = d4map_array_maps.begin(); it_dm != d4map_array_maps.end(); ++it_dm) 
        it_dm->second = nullptr;

 
}




// Mainly copy from HDF5CF::get_CF_string. Should be 
// removed if we can generate DMR independently.
string get_cf_string(string & s) {

    if(s[0] !='/') 
        return get_cf_string_helper(s);
    else {
        // The leading underscore should be removed 
        s.erase(0,1);
        return get_cf_string_helper(s);
    }

}
string get_cf_string_helper(string & s) {

    if ("" == s) return s;
    string insertString(1, '_');

    // Always start with _ if the first character is not a letter
    if (true == isdigit(s[0])) s.insert(0, insertString);

    for (unsigned int i = 0; i < s.length(); i++)
        if ((false == isalnum(s[i])) && (s[i] != '_')) s[i] = '_';
    return s;
}
