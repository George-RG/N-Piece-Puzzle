# Βρίσκουμε ποιο λειτουργικό σύστημα χρησιμοποιείται
ifdef WASM
	SUFFIX = wasm
endif

all: libraylib.a

libraylib.a:
	cp libraylib_$(SUFFIX).a libraylib.a

clean:
	$(RM) libraylib.a	