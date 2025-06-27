# Variables
CC = g++
#CFLAGS = -O2 -pthread -Iwhisper -Iportaudio -fopenmp
#LDFLAGS = -O2 -pthread -lws2_32 -lportaudio -L. -lwhisper -l:ggml.a -l:ggml-base.a -l:ggml-cpu.a -fopenmp
CFLAGS = -O2 -pthread  -fopenmp
LDFLAGS =  -O2 -pthread -lws2_32 -L. -fopenmp

RM = rm -f

# Si STATIC=1, se agrega la opción -static
ifeq ($(STATIC),1)
    LDFLAGS += -static
endif

# Soporte para compilación de 32 bits
ifeq ($(ARCH),32)
    CFLAGS += -m32
    LDFLAGS += -m32
endif

# Archivos fuente y objetos
SRC = main.cpp httplib_wrapper.cpp autoexec.cpp ini.cpp utils.cpp speak.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = harry

# Regla por defecto
all: $(TARGET)

# Regla para enlazar
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Regla para compilar cada archivo fuente
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza de archivos generados
clean:
	$(RM) $(OBJ) $(TARGET)