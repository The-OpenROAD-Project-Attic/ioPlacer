PARALLEL = 1

default: normal

all: clean release

normal: dirs
	@( \
		echo Change to build ;\
		cd build ;\
		echo Call cmake ;\
		cmake $(CM_OPT) .. ;\
		echo Call make ;\
		make $(MK_OPT) -j$(PARALLEL) --no-print-directory ;\
		)


release: dirs
	@( \
		echo Change to build ;\
		cd build ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Release .. ;\
		echo Call make ;\
		make $(MK_OPT) -j$(PARALLEL) --no-print-directory ;\
		)

debug: dirs
	@( \
		echo Change to build ;\
		cd build ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Debug .. ;\
		echo Call make ;\
		make $(MK_OPT) -j$(PARALLEL) --no-print-directory ;\
		)

dirs:
	@( \
		echo Create build ;\
		mkdir -p build ;\
		)

clean:
	@rm -rf build
