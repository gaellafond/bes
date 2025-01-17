
# Tests

AUTOMAKE_OPTIONS = foreign

# Because cacheT and uncompressT don't work with distcheck and a
# parallel build. :-( jhrg 8/22/17
.NOTPARALLEL:

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir)/dispatch $(DAP_CFLAGS)
LDADD = $(top_builddir)/dispatch/libbes_dispatch.la $(DAP_LIBS)
# LDADD = /Users/ndp/OPeNDAP/hyrax/bes/dispatch/.libs/libbes_dispatch.dylib  $(DAP_LIBS)
# LDADD = $(DAP_LIBS) $(top_builddir)/dispatch/.libs/libbes_dispatch.a $(BES_BZ2_LIBS)


if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
LDADD += $(CPPUNIT_LIBS)
endif

# These are not used by automake but are often useful for certain types of debugging.
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align

AM_CXXFLAGS=
AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

check_PROGRAMS = $(TESTS)

noinst_HEADERS = test_config.h test_utils.h

BUILT_SOURCES = test_config.h bes.conf

if CPPUNIT
# This determines what gets run by 'make check.'
TESTS = constraintT defT keysT pfileT plistT pvolT replistT		\
reqhandlerT reqlistT resplistT infoT utilT regexT scrubT		\
checkT servicesT fsT urlT containerT uncompressT cacheT			\
BESCatalogListTest CatalogNodeTest CatalogItemTest \
ServerAdministratorTest kvp_utils_test \
RequestTimerTest

# Debugging code is compiled out of the project when
# BES_DEVELOPER is not set, and so running the debugT tests
# produces no useful result.
if BES_DEVELOPER
TESTS += debugT
endif

# This is a tool to look at CatalogEntry objects. jhrg 3.5.18
# complete_catalog_lister agglistT 

if LIBDAP
TESTS += catT
endif

else

TESTS =

check-local:
	@echo ""
	@echo "**********************************************************"
	@echo "You must have cppunit 1.12.x or greater installed to run *"
	@echo "check target in dispatch unit-tests directory            *"
	@echo "**********************************************************"
	@echo ""
endif

# Limit the files copied to just these, eliminating the files created when tests are run.
# jhrg 5/13/07
CACHE = cache/template.txt cache/testfile.txt.bz2 cache/testfile.txt.gz cache/testfile.txt.Z

EXTRA_DIST = $(CACHE) catalog_test catalog_test_baselines bad_keys1.ini \
	defT.ini empty.ini info_test.ini keys_test.ini \
	keys_test_include.ini keys_test_m1.ini \
	keys_test_m2.ini keys_test_m3.ini \
	keys_test_map.ini keys_test_map_map.ini keys_test_vector.ini \
	persistence_cgi_test.ini persistence_file_test.ini \
	persistence_mysql_test.ini container01.file \
	persistence_file1.txt persistence_file2.txt \
	persistence_file3.txt persistence_file4.txt \
	persistence_file_test.txt test_config.h.in \
	bes.conf.in

DISTCLEANFILES = 

CLEANFILES = *.log *.sum real* test_config.h bes.conf

############################################
# This was generating a directory of "." so 
# I migrated it to the abs_srcdir below 
# - ndp 4/12/17
#
#test_config.h: test_config.h.in Makefile
#	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > test_config.h
#
# Changed test_config.h.in tp $(srcdir)/test_config.h.in. jhrg 1/21/18
test_config.h: $(srcdir)/test_config.h.in Makefile
	@mod_abs_srcdir=`${PYTHON} -c "import os.path; print(os.path.abspath('${abs_srcdir}'))"`; \
	mod_abs_top_srcdir=`${PYTHON} -c "import os.path; print(os.path.abspath('${abs_top_srcdir}'))"`; \
	mod_abs_builddir=`${PYTHON} -c "import os.path; print(os.path.abspath('${abs_builddir}'))"`; \
	sed -e "s%[@]abs_srcdir[@]%$${mod_abs_srcdir}%" \
		-e "s%[@]abs_top_srcdir[@]%$${mod_abs_top_srcdir}%" \
		-e "s%[@]abs_builddir[@]%$${mod_abs_builddir}%" $< > test_config.h

bes.conf: $(srcdir)/bes.conf.in Makefile
	@mod_abs_srcdir=`${PYTHON} -c "import os.path; print(os.path.abspath('${abs_srcdir}'))"`; \
	mod_abs_top_srcdir=`${PYTHON} -c "import os.path; print(os.path.abspath('${abs_top_srcdir}'))"`; \
	mod_abs_builddir=`${PYTHON} -c "import os.path; print(os.path.abspath('${abs_builddir}'))"`; \
	sed -e "s%[@]abs_srcdir[@]%$${mod_abs_srcdir}%" \
		-e "s%[@]abs_top_srcdir[@]%$${mod_abs_top_srcdir}%" \
		-e "s%[@]abs_builddir[@]%$${mod_abs_builddir}%" $< > bes.conf

clean-local:
	cd $(srcdir)/cache && rm -f *_cache*
	rm -rf test_cache_64
	rm -rf testdir

############################################################################

kvp_utils_test_SOURCES = kvp_utils_test.cc
kvp_utils_test_LDADD = ../kvp_utils.o $(LDADD)

constraintT_SOURCES = constraintT.cc

containerT_SOURCES = containerT.cc

defT_SOURCES = defT.cc

keysT_SOURCES = keysT.cc
# keysT_LDADD = ../TheBESKeys.o $(LDADD)

pfileT_SOURCES = pfileT.cc

plistT_SOURCES = plistT.cc

pvolT_SOURCES = pvolT.cc

replistT_SOURCES = replistT.cc TestReporter.cc TestReporter.h

reqhandlerT_SOURCES = reqhandlerT.cc TestRequestHandler.cc TestRequestHandler.h

reqlistT_SOURCES = reqlistT.cc TestRequestHandler.cc TestRequestHandler.h

resplistT_SOURCES = resplistT.cc TestResponseHandler.cc TestResponseHandler.h

infoT_SOURCES = infoT.cc
infoT_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)

# agglistT_SOURCES = agglistT.cc TestAggServer.cc TestAggServer.h

cacheT_SOURCES = cacheT.cc

uncompressT_SOURCES = uncompressT.cc

debugT_SOURCES = debugT.cc

utilT_SOURCES = utilT.cc

urlT_SOURCES = urlT.cc

regexT_SOURCES = regexT.cc

scrubT_SOURCES = scrubT.cc

checkT_SOURCES = checkT.cc

CatalogNodeTest_SOURCES = test_utils.cc CatalogNodeTest.cc
CatalogNodeTest_LDADD = ../CatalogNode.o $(LDADD)

CatalogItemTest_SOURCES = test_utils.cc CatalogItemTest.cc
CatalogItemTest_LDADD = ../CatalogItem.o $(LDADD)

servicesT_SOURCES = servicesT.cc
servicesT_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)

fsT_SOURCES = fsT.cc

if LIBDAP
catT_OBJ = ../BESCatalogResponseHandler.o
catT_SOURCES = test_utils.cc catT.cc
catT_CPPFLAGS = $(AM_CPPFLAGS) $(XML2_CFLAGS)
catT_LDADD = $(catT_OBJ) $(LDADD)
endif

# Added Unit test for singleton class BESCatalogList. ndp 5/6/2013
BESCatalogListTest_SOURCES  = BESCatalogListTest.cc
BESCatalogListTest_CPPFLAGS = $(AM_CPPFLAGS) -Wno-deprecated

# complete_catalog_lister_SOURCES = complete_catalog_lister.cc
# complete_catalog_lister_OBJ = ../BESCatalogResponseHandler.o
# complete_catalog_lister_CPPFLAGS =  $(AM_CPPFLAGS) $(XML2_CFLAGS)
# complete_catalog_lister_LDADD = $(complete_catalog_lister_OBJ) $(LDADD)

ServerAdministratorTest_SOURCES = ServerAdministratorTest.cc
ServerAdministratorTest_LDADD = ../ServerAdministrator.o $(LDADD)

RequestTimerTest_SOURCES = RequestTimerTest.cc
