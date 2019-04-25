BIN_NAME = ioPlacement

BIN_ARGS =  -f ispd18_test2.fpn -n ispd18_test2.nets -x 200 -y 190 -t 20 -r 30 -d out.def

PRE_ARGS = timeout 1s

BUILD_DIR = build

PARALLEL = 1

TYPE = normal

default: $(TYPE)

all: clean release

run:
	/usr/bin/time -v ./$(BIN_NAME) $(BIN_ARGS)

debug: dirs cmake_debug call_make
normal: dirs cmake_normal call_make
release: dirs cmake_release call_make

call_make:
	@echo Call make
	@make -C $(BUILD_DIR) $(MK_OPT) -j$(PARALLEL) --no-print-directory
	@echo Remove old binary
	@rm -f $(BIN_NAME)
	@echo Copy binary
	@cp $(BUILD_DIR)/$(BIN_NAME) .

cmake_normal:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) .. ;\
		)


cmake_release:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Release .. ;\
		)

cmake_debug:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Debug .. ;\
		)

dirs:
	@( \
		echo Create $(BUILD_DIR) ;\
		mkdir -p $(BUILD_DIR) ;\
		)

clean:
	rm -rf $(BUILD_DIR)

clean_all:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
