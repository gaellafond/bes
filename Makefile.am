# Build the Back End Server.
#
# jhrg 9/2/05
#
# Parallel versus serial build times:
# A full build using 'make' on a four core single-processor laptop took:
# real    2m7.363s
# user    1m51.920s
# sys     0m13.313s
#
# on the same machine, the same build using 'make -j6' took:
# real    0m48.181s
# user    3m29.876s
# sys     0m23.857s

AUTOMAKE_OPTIONS = foreign check-news subdir-objects
ACLOCAL_AMFLAGS = -I conf

AM_DISTCHECK_CONFIGURE_FLAGS=

if WITH_DEPENDENCIES
# With automake 1.14 setting AM_DISTCHECK... means that the user variable
# DISTCHECK_CONFIGURE_FLAGS no longer needs to be used. Earlier versions
# of automake don't support this. jhrg 6/5/17
AM_DISTCHECK_CONFIGURE_FLAGS += --with-dependencies=@ac_bes_dependencies_prefix@ --enable-developer
endif

AM_CXXFLAGS=
AM_LDFLAGS =
include $(top_srcdir)/coverage.mk

SUBDIRS = dispatch xmlcommand ppt server http cmdln standalone docs bin templates hello_world

if LIBDAP
SUBDIRS += dap dapreader

if DAP_BUILTIN_MODULES
SUBDIRS += modules
# moved functions to the modules directory. jhrg 12.22.21
endif

endif

bin_SCRIPTS = bes-config bes-config-pkgconfig bes-tmpfiles-conf

# Yes, even though its `init_d_SCRIPTS`, we use `init_ddir`,
# with-out a second underscore.
init_ddir = $(sysconfdir)/rc.d/init.d
init_d_SCRIPTS = besd

logrotate_ddir = $(sysconfdir)/logrotate.d
logrotate_d_DATA = besd.logrotate

CLEANFILES = $(init_d_SCRIPTS)

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = bes_dispatch.pc bes_ppt.pc bes_xml_command.pc

aclocaldir=$(datadir)/aclocal
dist_aclocal_DATA = conf/bes.m4

EXTRA_DIST = doxy.conf.in bes-config-pkgconfig \
bes-tmpfiles-conf besd.logrotate README.md rapidjson pugixml

HTML_DOCS=html
DOXYGEN_CONF=doxy.conf

framework = dispatch xmlcommand ppt server http dap dapreader
.PHONY: framework-only
framework-only:
	for d in $(framework); do $(MAKE) -C $$d $(MFLAGS); done

# Added this as a simple way to test the framework without the modules.
# jhrg 3.18.22
.PHONY: framework-check
framework-check:
	for d in $(framework); do $(MAKE) check -C $$d $(MFLAGS); done

# This target only builds the documents, it does not move them to
# github. Use the 'gh-docs' target below for that.
.PHONY: docs
docs: ${DOXYGEN_CONF}
	doxygen ${DOXYGEN_CONF}

# GitHub.io docs 
.PHONY: gh-docs 
gh-docs:
	./build-gh-docs.sh

# cccc computes metrics like Lines of code and McCabe. It'a available
# on the web...
# For the BES, compute metrics for: dispatch xmlcommand ppt server
.PHONY: cccc
cccc:	
	for d in dispatch xmlcommand ppt server; \
		do $(MAKE) -C $$d $(MFLAGS) cccc; \
	done

# This hack strips out the individual handlers' spec files from the
# big tar and lets us use the old target again, which means I don't
# have to code the rpmbuild root into a target.
dist-hook:
	-rm -rf `find $(distdir) -name '*.spec' -a ! -name bes.spec `
	-rm -rf `find $(distdir) -name 'test_config.h' `

# Build linux RPMs

srpm: dist
	rpmbuild -ts --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

# NB: 'dist' builds a tar.gz package using automake in the CWD.
rpm: dist
	cp @PACKAGE@-@VERSION@.tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb --clean $(RPM_OPTIONS) bes.spec

# This target is pretty specific to our need to build a RPM for use at
# NASA DAACs. We are assuming that the user has the rpmbuild dirs.
# This will link with everything it can find in the local deps dir,
# removing the need for EPEL
all-static-rpm: dist
	cp @PACKAGE@-@VERSION@.tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb $(RPM_OPTIONS) bes.spec.all_static

###########################################################################

#Build the debian package
deb: dist 
	( mv @PACKAGE@-@VERSION@.tar.gz ../@PACKAGE@_@VERSION@.orig.tar.gz )
	( cd ../ && tar xzf @PACKAGE@_@VERSION@.orig.tar.gz )
	( cp -r debian/ ../@PACKAGE@-@VERSION@ )
	( cd ../@PACKAGE@-@VERSION@ && debuild --preserve-env --prepend-path=$(prefix)/deps/bin -us -uc -j7)

##########################################################################

# Build OS/X Packages. The strange operations with configure and make
# are there so that the values built into bes-config will match the mac
# osx install dirs and not the temp directory used to build the packages

PACKAGEMAKER=/Developer/usr/bin/packagemaker
PKG_CONF_FLAGS=

clean-pkg:
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@

pkg-build: clean-pkg
	./configure --prefix=$(prefix) --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=`pwd`/mac_osx make install

pkg-main: pkg-build
	./OSX_Resources/update_mac_package_contents.pl README
	cat ./OSX_Resources/InstallationCheck.proto | sed -e "s^_PREFIX_^$(prefix)^g" > foo
	mv foo ./OSX_Resources/InstallationCheck
	cat ./OSX_Resources/Info.plist.proto | \
	   sed -e "s^_PREFIX_^$(prefix)^g" \
	       -e "s^_FULL_VERSION_^@PACKAGE_VERSION@^g" \
	       -e "s^_MAJOR_VERSION_^@PACKAGE_MAJOR_VERSION@^g" \
	       -e "s^_MINOR_VERSION_^@PACKAGE_MINOR_VERSION@^g" \
	       > foo
	mv foo ./OSX_Resources/Info.plist
	${PACKAGEMAKER} --root mac_osx --id org.opendap.@PACKAGE@ \
		--title "@PACKAGE@ @PACKAGE_VERSION@" --version @PACKAGE_VERSION@ \
		--out @PACKAGE@-@PACKAGE_VERSION@.pkg --resources OSX_Resources

pkg-dmg: pkg-main
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	cp -r @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp README NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: pkg-main 

