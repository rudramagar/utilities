CXX      = g++
CXXFLAGS = -O2 -Wall -Werror
TARGET   = bmd_decoder
SRC      = bmd_decoder.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean install
