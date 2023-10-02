include /opt/fpp/src/makefiles/common/setup.mk

all: libfpp-plugin-genius.so
debug: all


OBJECTS_fpp_genius_so += src/geniusPlugin.o src/genius_controller.o
LIBS_fpp_genius_so += -L/opt/fpp/src -lfpp
CXXFLAGS_src/geniusPlugin.o += -I/opt/fpp/src

%.o: %.cpp Makefile
	$(CCACHE) $(CC) $(CFLAGS) $(CXXFLAGS) $(CXXFLAGS_$@) -c $< -o $@

libfpp-plugin-genius.so: $(OBJECTS_fpp_genius_so) /opt/fpp/src/libfpp.so
	$(CCACHE) $(CC) -shared $(CFLAGS_$@) $(OBJECTS_fpp_genius_so) $(LIBS_fpp_genius_so) $(LDFLAGS) -o $@

clean:
	rm -f libfpp-plugin-genius.so $(OBJECTS_fpp_genius_so)
