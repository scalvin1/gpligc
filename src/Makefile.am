# src/Makefile.am for the gpligc/ogie project

bin_PROGRAMS = ogie merger optimizer etopo1merger etopo2merger createworld
ogie_SOURCES = GLexplorer.cpp GLexplorer.h airspc.cpp airspc.h cmdline.c cmdline.h \
		oconfig.cpp oconfig.h ego.cpp ego.h flightdata.cpp flightdata.h \
		landscape.cpp landscape.h map.cpp map.h oglexfunc.cpp oglexfunc.h \
		points.cpp points.h projection.cpp projection.h value.h write_jpeg.c
ogie_LDFLAGS = -Wl,--as-needed

merger_SOURCES = merger.cpp
merger_LDFLAGS = -Wl,--as-needed

optimizer_SOURCES = optimizer.cpp oglexfunc.cpp
optimizer_CXXFLAGS = -DOPTIMIZER
optimizer_LDFLAGS = -Wl,--as-needed

etopo1merger_SOURCES = etopo1merger.cpp
etopo1merger_LDFLAGS = -Wl,--as-needed

etopo2merger_SOURCES = etopo2merger.cpp
etopo2merger_LDFLAGS = -Wl,--as-needed

createworld_SOURCES = createworlddem.cpp
createworld_LDFLAGS = -Wl,--as-needed

sharedir = $(datadir)/gpligc
dist_share_DATA = .ogierc
