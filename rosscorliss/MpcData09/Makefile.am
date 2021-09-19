AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(includedir) -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libpmmpc13.la 

include_HEADERS =

libpmmpc13_la_LIBADD = \
  -L$(libdir) \
  -L${ROOTSYS}/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lphflag \
  -lphool \
  -lphtime \
  -lPISARoot \
  -lSubsysReco \
  -lTOAD \
  -lmpc 

libpmmpc13_la_SOURCES = \
  PmMpc13.cc \
  PmMpc13_Dict.C

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libpmmpc13.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating CINT dictionaries from class headers.
PmMpc13_Dict.C: \
	PmMpc13.hh \
	PmMpc13LinkDef.h
		rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

dist_pkgdata_DATA = \
	final_gain_ratios.txt

testexternals_SOURCES = \
  testexternals.C

clean-local:
	rm -f *Dict*
