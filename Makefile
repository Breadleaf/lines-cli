CC := $(shell which gcc)
C_FLAGS := -Wall -Werror -Wpedantic -std=c11
C_FILES := $(wildcard *.c)
H_FILES := $(wildcard *.h)
CLANG_FORMAT := $(shell which clang-format)
EXEC_NAME := lines
INSTALL_DIR := $(HOME)/.local/bin/

SHELL_NAME := $(shell basename $$SHELL)
ifeq ($(SHELL_NAME),bash)
	RCFILE := ~/.bashrc
else ifeq ($(SHELL_NAME),zsh)
	RCFILE := ~/.zshrc
else ifeq ($(SHELL_NAME),fish)
	RCFILE := $${XDG_CONFIG_HOME:-~/.config}/fish/config.fish
else
	RCFILE := ~/.profile
endif

.PHONY: help tips build install remove format

all: help

help:
	@echo "usage: make [target]"
	@echo "- help    : print this message"
	@echo "- tips    : print program tips"
	@echo "- build   : build the project"
	@echo "- install : build the project and move exec to $(INSTALL_DIR)"
	@echo "- remove  : delete project from $(INSTALL_DIR)"
	@echo "- format  : format the project with clang-format"

tips:
	@echo "If you choose to install the project to $(INSTALL_DIR), you may want to run:"
	@echo
	@echo "    echo 'export PATH=\"\$$HOME/.local/bin:\$$PATH\"' >> $(RCFILE)"
	@echo
	@echo "Then reload with:"
	@echo
	@echo "    source $(RCFILE)"

build:
	$(CC) $(C_FLAGS) $(C_FILES) -o $(EXEC_NAME)

install: build
	mkdir -p $(INSTALL_DIR)
	@if [ -e "$(INSTALL_DIR)$(EXEC_NAME)" ]; then \
		printf "overwrite $(INSTALL_DIR)$(EXEC_NAME)? (y/n) "; \
		read ans; \
		if [ "$$ans" = "y" ]; then \
			mv ./$(EXEC_NAME) "$(INSTALL_DIR)/"; \
			echo; \
			echo "WAIT! Please read the below section:"; \
			echo; \
			$(MAKE) tips; \
		else \
			echo "Not overwritten."; \
		fi \
	else \
		mv ./$(EXEC_NAME) "$(INSTALL_DIR)/"; \
		echo; \
		echo "WAIT! Please read the below section:"; \
		echo; \
		$(MAKE) tips; \
	fi

remove:
	rm -i ~/.local/bin/$(EXEC_NAME)

format:
	$(CLANG_FORMAT) -i $(C_FILES) $(H_FILES) --verbose \
		--style="{BasedOnStyle: Microsoft, BreakBeforeBraces: Attach, ColumnLimit: 80}"
