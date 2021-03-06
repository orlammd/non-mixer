# -*- mode: makefile; -*-

all: Mixer

Mixer_SRCS := $(wildcard src/*.C src/*.fl src/Engine/*.C)

Mixer_SRCS:=$(Mixer_SRCS:.fl=.C)
Mixer_SRCS:=$(sort $(Mixer_SRCS))
Mixer_OBJS:=$(Mixer_SRCS:.C=.o)

Mixer_LIBS := $(FLTK_LIBS) $(JACK_LIBS) $(LASH_LIBS) $(LRDF_LIBS)

src/mixer: $(Mixer_OBJS) FL/libfl_widgets.a nonlib/libnonlib.a
	@ echo -n Linking mixer...
	@ $(CXX) $(CXXFLAGS) $(Mixer_LIBS) $(Mixer_OBJS) -o $@ -LFL -lfl_widgets -Lnonlib -lnonlib -ldl -llo -lpthread && echo $(DONE)

Mixer: src/mixer

Mixer_clean:
	rm -f $(Mixer_OBJS) src/mixer
