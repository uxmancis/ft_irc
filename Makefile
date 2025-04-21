NAME	=	ircserv

CC		=	g++
FLAGS	=	-Wall -Wextra -Werror -std=c++98

SRC		=	main.cpp	\
 
OBJS	=	$(SRC:.cpp=.o)

YELLOW	=	\033[1;33m
CYAN	=	\033[1;36m
GREEN	=	\033[1;32m
RESET	=	\033[0m

all: $(NAME)

$(NAME) : $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Program $(NAME) compiled successfully!$(RESET)"
	@echo "$(CYAN)												 "
	@echo "   ___  _____    _____   __    ___ 					 "
	@echo "  / __\/__   \   \_   \ /__\  / __\					 "
	@echo " / _\    / /\/    / /\// \// / /   					 "
	@echo "/ /     / /    /\/ /_ / _  \/ /___ 					 "
	@echo "\/      \/_____\____/ \/ \_/\____/ 					 "
	@echo "         |_____|                   					 "
	@echo "														 "
	@echo "by ngastana && emunoz && uxmancis					 "
	@echo "	$(RESET)											 "
                                                    
                                                    
%.o: %.cpp
	@echo "$(YELLOW)Compiling $<$(RESET)"
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@echo "$(CYAN)Object files removed.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(CYAN)Program $(NAME) removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re
