AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(includedir) -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include

lib_LTLIBRARIES = \
  librcmpc09.la 

include_HEADERS =

librcmpc09_la_LIBADD = \
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

librcmpc09_la_SOURCES = \
  RcMpc09.cc \
  RcMpc09_Dict.C

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  librcmpc09.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating CINT dictionaries from class headers.
RcMpc09_Dict.C: \
	RcMpc09.hh \
	RcMpc09LinkDef.h
		rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

dist_pkgdata_DATA = \
	final_gain_ratios.txt

testexternals_SOURCES = \
  testexternals.C

clean-local:
	rm -f *Dict*
