NAME = asm

#########
RM = rm -rf
CC = cc
LIB_PATHS = -L../libs/libft -L../libs/printf -L../libs/util
LIB_INCLUDES = -I../libs/libft -I../libs/printf -I../libs/util -Iinc
CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 $(LIB_INCLUDES) $(LIB_PATHS)
# CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -I$(OPENSSL_BUILD_DIR)/include -I$(THIRD_PARTY_PATH)/cJSON -Iinc -DUSE_SSL #-DDEBUG
# CFLAGS = -Werror -Wextra -Wall -g -fsanitize=thread -O3 -I$(OPENSSL_BUILD_DIR)/include -I$(THIRD_PARTY_PATH)/cJSON -Iinc -DUSE_SSL #-DDEBUG
LDFLAGS = -lm -lutil -lft -lftprintf
RELEASE_CFLAGS = -Werror -Wextra -Wall -g -O3
#########

LIBFT_A = ../libs/libft/libft.a
PRINTF_A = ../libs/printf/libftprintf.a
UTIL_A = ../libs/util/libutil.a
LIBS_A = $(LIBFT_A) $(PRINTF_A) $(UTIL_A)

#########
FILES = main parse op encode parse_utils parse_utils2 parse_arg parse_specials \
		handle_label offsets instr eval_expr normalize_args encode_utils

SRC = $(addsuffix .c, $(FILES))

vpath %.c srcs inc ../op srcs/parser srcs/encode
#########

OBJ_DIR = objs

#########
#########

#########
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
DEP = $(addsuffix .d, $(basename $(OBJ)))
#########

#########
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	${CC} -MMD $(CFLAGS) -c $< -o $@


all: .gitignore $(LIBS_A)
	$(MAKE) $(NAME)

debug: CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -Iinc -DDEBUG #-lpthread -DDEBUG
debug: all
	@echo "DEBUG BUILD DONE  "

dre: CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -Iinc -DDEBUG #-lpthread -DDEBUG
dre: re
	@echo "DEBUG REBUILD DONE  "

$(LIBFT_A):
	$(MAKE) -C ../libs/libft

$(PRINTF_A):
	$(MAKE) -C ../libs/printf

$(UTIL_A):
	$(MAKE) -C ../libs/util

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -ldl -lpthread -o $(NAME) $(LDFLAGS)
	@echo "EVERYTHING DONE  "
#	@./.add_path.sh

release: CFLAGS = $(RELEASE_CFLAGS)
release: re
	@echo "RELEASE BUILD DONE  "

clean:
	$(RM) $(OBJ) $(DEP)
	$(RM) -r $(OBJ_DIR)
	$(MAKE) -C ../libs/libft clean
	$(MAKE) -C ../libs/printf clean
	$(MAKE) -C ../libs/util clean
	@echo "OBJECTS REMOVED   "

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C ../libs/libft fclean
	$(MAKE) -C ../libs/printf fclean
	$(MAKE) -C ../libs/util fclean
	@echo "EVERYTHING REMOVED   "

re: fclean
	$(MAKE) all CFLAGS="$(CFLAGS)"

.gitignore:
	@if [ ! -f .gitignore ]; then \
		echo ".gitignore not found, creating it..."; \
		echo ".gitignore" >> .gitignore; \
		echo "$(NAME)" >> .gitignore; \
		echo "$(OBJ_DIR)/" >> .gitignore; \
		echo "*.a" >> .gitignore; \
		echo "*.pem" >> .gitignore; \
		echo ".vscode/" >> ../.gitignore; \
		echo ".gitignore" >> ../.gitignore; \
		echo "log.txt" >> ../.gitignore; \
		echo ".gitignore created and updated with entries."; \
	else \
		echo ".gitignore already exists."; \
	fi

.PHONY: all clean fclean re release .gitignore debug dre


-include $(DEP)
