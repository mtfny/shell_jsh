CC = gcc
CFLAGS = 
LDFLAGS = -lm -lreadline

# Liste des fichiers source
SRCS = main.c commandes.c job.c redirection.c

# Liste des fichiers objets générés à partir des fichiers source
OBJS = $(SRCS:.c=.o)

# Nom de l'exécutable
EXEC = jsh

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean


