BIN_NAME = ioPlacement

BIN_ARGS =  -f ispd18_test2.fpn -n ispd18_test2.nets -d out.def

PRE_ARGS = timeout 1s

BUILD_DIR = build

PARALLEL = 1

default: normal

all: clean release

run: normal
	-$(PRE_ARGS) ./$(BIN_NAME) $(BIN_ARGS)

normal: dirs
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) .. ;\
		echo Call make ;\
		make $(MK_OPT) -j$(PARALLEL) --no-print-directory ;\
		echo Remove old binary ;\
		rm -f ../$(BIN_NAME) ;\
		echo Copy binary ;\
		cp $(BIN_NAME) .. ;\
		)


release: dirs
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Release .. ;\
		echo Call make ;\
		make $(MK_OPT) -j$(PARALLEL) --no-print-directory ;\
		echo Remove old binary ;\
		rm -f ../$(BIN_NAME) ;\
		echo Copy binary ;\
		cp $(BIN_NAME) .. ;\
		)

debug: dirs
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Debug .. ;\
		echo Call make ;\
		make $(MK_OPT) -j$(PARALLEL) --no-print-directory ;\
		echo Remove old binary ;\
		rm -f ../$(BIN_NAME) ;\
		echo Copy binary ;\
		cp $(BIN_NAME) .. ;\
		)

dirs:
	@( \
		echo Create $(BUILD_DIR) ;\
		mkdir -p $(BUILD_DIR) ;\
		)

clean:
	rm -rf $(BUILD_DIR)
