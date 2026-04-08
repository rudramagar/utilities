CXX      = g++
CXXFLAGS = -O2 -Wall -Werror
TARGET   = time_stamp_parse
SRC      = time_stamp_parse.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean install
