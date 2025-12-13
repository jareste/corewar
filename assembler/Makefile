NAME = asm

#########
RM = rm -rf
CC = cc
CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -Iinc #-lpthread #-DDEBUG
# CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -I$(OPENSSL_BUILD_DIR)/include -I$(THIRD_PARTY_PATH)/cJSON -Iinc -DUSE_SSL #-DDEBUG
# CFLAGS = -Werror -Wextra -Wall -g -fsanitize=thread -O3 -I$(OPENSSL_BUILD_DIR)/include -I$(THIRD_PARTY_PATH)/cJSON -Iinc -DUSE_SSL #-DDEBUG
LDFLAGS = -L$(OPENSSL_BUILD_DIR)/lib -lssl -lcrypto -lm
RELEASE_CFLAGS = -Werror -Wextra -Wall -g -O3
#########

#########
FILES = main ft_list ft_malloc parse log op encode parse_utils parse_utils2 parse_arg \
		 offsets instr eval_expr normalize_args

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


all: .gitignore
	
	$(MAKE) $(NAME)

debug: CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -Iinc -DDEBUG #-lpthread -DDEBUG
debug: all
	@echo "DEBUG BUILD DONE  "

dre: CFLAGS = -Werror -Wextra -Wall -g -fsanitize=address -O3 -Iinc -DDEBUG #-lpthread -DDEBUG
dre: re
	@echo "DEBUG REBUILD DONE  "

$(NAME): $(OBJ) Makefile
	$(CC) $(CFLAGS) $(OBJ) -ldl -lpthread -o $(NAME) $(LDFLAGS)
	@echo "EVERYTHING DONE  "
#	@./.add_path.sh

$(CJSON_OBJ): $(CJSON_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CJSON_LIB): $(CJSON_OBJ)
	ar rcs $@ $<

release: CFLAGS = $(RELEASE_CFLAGS)
release: re
	@echo "RELEASE BUILD DONE  "

clean:
	$(RM) $(OBJ) $(DEP)
	$(RM) -r $(OBJ_DIR)
	@echo "OBJECTS REMOVED   "

fclean: clean
	$(RM) $(NAME)
	cd $(OPENSSL_SRC_DIR) && [ -f Makefile ] && make clean || true
	rm -rf $(OPENSSL_INSTALL_DIR)
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

.PHONY: all clean fclean re release .gitignore


-include $(DEP)