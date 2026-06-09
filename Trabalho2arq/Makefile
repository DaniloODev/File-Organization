# Variáveis de configuração
CC = gcc
# -I. diz para procurar .h na raiz, os outros buscam nas subpastas
FLAGS = -Wall -Werror -g -I. -I./fornecidas -I./auxiliar -I./funcoesprincipais -I./estrutura_verifica
LIBS = -lm

# Organização dos arquivos fonte (.c)
MAIN = main.c
FORNECIDAS = ./fornecidas/fornecidas.c
AUX = ./auxiliar/auxiliares.c ./auxiliar/calcula.c ./auxiliar/printaEstacao.c
PRINCIPAL = ./funcoesprincipais/select.c ./funcoesprincipais/create_table.c \
            ./funcoesprincipais/update.c ./funcoesprincipais/delete.c \
            ./funcoesprincipais/insert_into.c ./funcoesprincipais/busca.c
STCTVRF = ./estrutura_verifica/dados.c ./estrutura_verifica/cabecalho.c

# Regra principal
all:
	$(CC) $(FLAGS) -o programaTrab $(MAIN) $(FORNECIDAS) $(AUX) $(PRINCIPAL) $(STCTVRF) $(LIBS)

run:
	./programaTrab

clean:
	rm -f programaTrab *.bin
