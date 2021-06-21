
# Automake file for csv_handler
#
# 5/17/2005 jhrg
#
# $Id: Makefile.am 12972 2006-01-05 15:41:07Z pwest $

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I conf

# Set the module version here, in the spec file and in configure.ac
M_NAME=csv_handler
M_VER=1.2.4

AM_CPPFLAGS = -I$(top_srcdir)/dispatch -I$(top_srcdir)/dap $(DAP_CFLAGS)
LIBADD = $(DAP_SERVER_LIBS) $(DAP_CLIENT_LIBS)

AM_CPPFLAGS += -DMODULE_NAME=\"$(M_NAME)\" -DMODULE_VERSION=\"$(M_VER)\"

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libcsv_module.la

AM_CXXFLAGS=
AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

SUBDIRS = . tests

CSV_SRCS = \
		CSVModule.cc CSVRequestHandler.cc			\
		CSV_Data.cc CSV_Header.cc CSV_Obj.cc CSV_Reader.cc	\
		CSVDAS.cc CSVDDS.cc CSV_Utils.cc

CSV_HDRS = \
		CSVModule.h CSVRequestHandler.h				\
		CSVDAS.h CSVDDS.h CSV_Data.h CSV_Field.h		\
		CSV_Header.h CSV_Obj.h CSV_Reader.h CSV_Utils.h

libcsv_module_la_SOURCES = $(CSV_SRCS) $(CSV_HDRS)
libcsv_module_la_LDFLAGS = -avoid-version -module 
libcsv_module_la_LIBADD = $(LIBADD)

EXTRA_DIST = data csv.conf.in 

CLEANFILES = *~ csv.conf

sample_datadir = $(datadir)/hyrax/data/csv
sample_data_DATA = data/temperature.csv

moduledir = $(sysconfdir)/bes/modules
module_DATA = csv.conf

csv.conf: csv.conf.in $(top_builddir)/config.status
	sed -e "s%[@]bes_modules_dir[@]%${lib_besdir}%" $< > csv.conf

C4_DIR=./cccc
.PHONY: cccc
cccc:	
	cccc --outdir=$(C4_DIR) $(libgateway_module_la_SOURCES)

# PyBind

# PYBIND11_INC is from: $(python3 -m pybind11 --includes)
PYBIND11_INC = -I/Library/Frameworks/Python.framework/Versions/3.7/include/python3.7m \
-I/Library/Frameworks/Python.framework/Versions/3.7/lib/python3.7/site-packages/pybind11/include

PYBIND_CPPFLAGS = $(PYBIND11_INC) -I$(top_srcdir)/dispatch -I$(top_srcdir) -I. $(DAP_CFLAGS)

PYBIND_CXXFLAGS = -O0 -g3 -Wall -std=c++14

# PYBIND11_EXT from: $(python3-config --extension-suffix)
PYBIND11_EXT = .cpython-39-darwin.so

PYBIND_LDFLAGS = -shared -undefined dynamic_lookup -L$(prefix)/lib -ldap .libs/libcsv_module.a \
../../dispatch/.libs/libbes_dispatch.a $(top_builddir)/dap/.libs/libdap_module.a \
../../../libdap4/.libs/libdapserver.a

# c++ -I$(top_srcdir)/dispatch -I$(top_srcdir) -I. $(DAP_CFLAGS) -O3 -Wall -shared -std=c++14
# -undefined dynamic_lookup $(python3 -m pybind11 --includes) pybindCSV.cpp
# -o pybindCSV$(python3-config --extension-suffix) -L$(prefix)/lib -ldap

pybindCSV:
	c++  $(PYBIND_CPPFLAGS) $(PYBIND_CXXFLAGS) pybindCSV.cpp -o pybindCSV$(PYBIND11_EXT) $(PYBIND_LDFLAGS)

