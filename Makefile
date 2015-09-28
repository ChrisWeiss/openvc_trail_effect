CXXFLAGS = -ansi -pedantic -pedantic-errors -Wall -g3 -O2 -D_ANSI_SOURCE_
CXXFLAGS += -fno-common \
	    -fvisibility=hidden \
	    -Wall \
	    -Wextra \
	    -Wformat=2 \
	    -Winit-self \
	    -Winline \
	    -Wpacked \
	    -Wpointer-arith \
	    -Wlarger-than-65500 \
	    -Wmissing-declarations \
	    -Wmissing-format-attribute \
	    -Wmissing-noreturn \
	    -Wredundant-decls \
	    -Wsign-compare \
	    -Wstrict-aliasing=2 \
	    -Wswitch-enum \
	    -Wundef \
	    -Wunreachable-code \
	    -Wunsafe-loop-optimizations \
	    -Wunused-but-set-variable \
	    -Wwrite-strings \
	    -Wp,-D_FORTIFY_SOURCE=2 \
	    -fstack-protector \
	    --param=ssp-buffer-size=4

CXXFLAGS += $(shell pkg-config --cflags opencv)
LDLIBS += $(shell pkg-config --libs opencv)

opencv_trail_effect: $(wildcard *.hpp)


# Some command lines to imitate different trail styles

blame_it_on_the_boogie: opencv_trail_effect
	./opencv_trail_effect -l 12 -s background -d fadeaccumulate

wtf: opencv_trail_effect
	./opencv_trail_effect -l -1 -s background -d copy

l_anima_vola: opencv_trail_effect
	./opencv_trail_effect -l 30 -s background -d copy -r

average: opencv_trail_effect
	./opencv_trail_effect -l 10 -s background -d average -B

clean:
	rm -f opencv_trail_effect vgdump gtk.suppression

test: opencv_trail_effect
	[ -f gtk.suppression ] || wget -nv https://people.gnome.org/~johan/gtk.suppression
	G_DEBUG=gc-friendly G_SLICE=always-malloc \
		valgrind --tool=memcheck --leak-check=full --leak-resolution=high \
		--num-callers=20 --log-file=vgdump \
		--suppressions=gtk.suppression ./opencv_trail_effect
