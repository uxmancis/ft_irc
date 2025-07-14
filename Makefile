NAME	=	ircserv

CC		=	g++
FLAGS	=	-Wall -Wextra -Werror -std=c++98

SRC		=	main.cpp						\
			Server/Server.cpp 				\
			Poll/PollManager.cpp 			\
			Client/Client.cpp 				\
			Channel/Channel.cpp 			\
			Commands/Commands.cpp 			\
			Commands/HandleInvite.cpp 		\
			Commands/HandleJoin.cpp 		\
			Commands/HandleKick.cpp 		\
			Commands/HandleMsg.cpp 			\
			Commands/HandleNick.cpp 		\
			Commands/HandleQuit.cpp 		\
			Commands/HandleTopic.cpp 		\
			Commands/HandleUser.cpp 		\
			Commands/HandleMode.cpp 		\
			Commands/HandlePass.cpp 		\
			Commands/HandleWho.cpp 			\
			Commands/HandlePing.cpp 		\
			Commands/HandlePart.cpp 		\
			Commands/HandleNames.cpp 		\
			Commands/HandleList.cpp 		\
			Commands/HandleNotice.cpp 		\
			Commands/HandlePong.cpp 		\
			Commands/HandleWhois.cpp 		\
			Commands/HandleMotd.cpp 		\


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
	@echo "by ngastana && emunoz && uxmancis                     "
                                                                                           
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
