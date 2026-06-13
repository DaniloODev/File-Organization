# Variáveis de configuração
CC = gcc
# -I. diz para procurar .h na raiz, os outros buscam nas subpastas
FLAGS = -Wall -Werror -g -I. -I./fornecidas -I./aux -I./funcoes_principais -I./structs -I./funcoes_arvb
LIBS = -lm

# Organização dos arquivos fonte (.c)
MAIN = main.c

FORNECIDAS = ./fornecidas/fornecidas.c

AUX = ./aux/auxiliares.c ./aux/calcula.c

PRINCIPAL = ./funcoes_principais/select.c ./funcoes_principais/create_table.c \
            ./funcoes_principais/update.c ./funcoes_principais/delete.c \
            ./funcoes_principais/insert_into.c

ARVORE_B = ./funcoes_arvb/create_index.c

STCTVRF = ./structs/dados.c ./structs/cabecalho.c ./structs/cabecalho_arvb.c ./structs/arvore_b.c

# Regra principal
all:
	$(CC) $(FLAGS) -o programaTrab $(MAIN) $(FORNECIDAS) $(AUX) $(PRINCIPAL) $(ARVORE_B) $(STCTVRF) $(LIBS)

run:
	./programaTrab

clean:
	rm -f programaTrab *.bin
