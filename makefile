INPUT_FILE = input.def
OUTPUT_FILE = out.def

BIN_NAME = ioPlacement
BIN_ARGS = -i $(INPUT_FILE) -o $(OUTPUT_FILE) -h 5 -v 6 -w 1

PRE_CMD = /usr/bin/time -v
POS_CMD = 2>&1 | tee $(INPUT_FILE).log

BUILD_DIR = build

PARALLEL = 1

TYPE = normal

default: $(TYPE)

all: clean release

run:
	$(PRE_CMD) ./$(BIN_NAME) $(BIN_ARGS) $(POS_CMD)

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
