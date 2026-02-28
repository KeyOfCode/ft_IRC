RM = rm -f
CC = c++
CPPFLAGS = -Wall -Wextra -Werror 

OBJ_DIR = objs
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))
SRCS = main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp src/Command.cpp src/Parser.cpp 
HEADERS = include/Server.hpp include/Client.hpp  include/Channel.hpp include/Command.hpp include/Parser.hpp

NAME = ircserv

GREEN = \033[1;32m
RESET = \033[0m

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	@echo "$(GREEN)Compiling objects...$(RESET)"
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Executable created: $(NAME)$(RESET)"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) -c $< -o $@
	@echo "$(GREEN)$< compiled!$(RESET)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

exe:
	@make
	./$(NAME) 6667 1234
test:
	@make
	./$(NAME) 1234 1234

val:
	@make
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(NAME) 6667 1234

clean:
	@echo "$(GREEN)Cleaning...$(RESET)"
	@$(RM) $(OBJS)
	@echo "$(GREEN)Clean complete.$(RESET)"

fclean: clean
	@echo "$(GREEN)Deleting $(NAME)...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@$(RM) $(NAME)
	@echo "$(GREEN)Deleted $(NAME)$(RESET)"

re: fclean all

.PHONY: all clean fclean re