NAME := ft_ping
GCC := gcc
CFLAGS=-Wall -Wextra -Werror

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

SRC_FILES := main.c args.c resolve.c socket.c utils.c stats.c core.c icmp.c output.c
OBJ_FILES := $(SRC_FILES:%.c=$(OBJ_DIR)/%.o)

INCLUDES := -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@echo "\nLinking $(NAME)..."
	$(GCC) $(CFLAGS) $(OBJ_FILES) -o $(NAME) -lm
	@echo "Build completed."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(GCC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "Removing object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Removing binary..."
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

tree:
	@echo "Project tree:"
	@tree