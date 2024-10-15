S = src

BUILD = build

SRCS = $(wildcard $(S)/*.cpp)  

INCDIR = inc

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INCDIR) -g

TARGET = WebServ

OBJS = $(SRCS:$(S)/%.cpp=$(BUILD)/%.o)

# Colors
BLUE		=	\033[34m
YELLOW		=	\033[33m
GREEN		=	\033[32m
WHITE		=	\033[0m

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	printf "$(GREEN)Compilation finished\n$(WHITE)"

$(BUILD)/%.o: $(S)/%.cpp $(INCDIR) | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	printf "$(BLUE)> $<$(WHITE)\n"

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -f $(OBJS)
	rm -rf uploads/*
	printf "$(YELLOW)Object files removed$(WHITE)\n"

fclean: clean
	rm -f $(TARGET)
	rm -rf $(BUILD)
	rm -rf uploads
	printf "$(YELLOW)Directory is cleaned$(WHITE)\n"

re: fclean all

.PHONY: all clean fclean re

.SILENT:

