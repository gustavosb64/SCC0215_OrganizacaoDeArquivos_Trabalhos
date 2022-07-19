#ifndef B_TREE_H
#define B_TREE_H


/*
 * Estrutura que contém os dados de cabeçalho de um arquivo de índices Árvore-B
*/
typedef struct b_Header B_Header;

/*
 * Estrutura que contém os dados de uma chave, sendo seu ID e sua 
 * referência no arquivo de dados
*/
typedef struct key Key;

/*
 * Estrutura que contém dados de uma página de Árvore-B
*/
typedef struct node Node;

/*
 * Inicializa um cabeçalho de Árvore-B
 *  @return:
 *      B_Header*: estrutura B_Header* inicializada com os valores padrão
*/
B_Header* initialize_btree_header();

/* 
 * Escreve o cabeçalho de Árvore-B com os dados referentes à b_header
 *  @param:
 *      FILE* file_btree_rw: arquivo de índice Árvore-B
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *  @return:
 *      0: nenhum erro ocorreu
*/
int write_btree_header(FILE *file_btree_rw, B_Header *b_header, int f_type);

/* 
 * Inicializa uma variável do tipo Node com valores padrão
 *  @param:
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      Node*: estrutura Node* inicializada com os dados padrão
*/
Node* initialize_node(int f_type);

/*
 * Lê cabeçalho de um arquivo binário de índices estilo Árvore-B
 *  @param:
 *      FILE* file_btree_r: arquivo de índice Árvore-B
 *  @return:
 *      B_Header*: estrutura do B_Header* com os dados lidos 
*/
B_Header* read_header_from_btree(FILE *file_btree_r);

/*
 * Lê um nó da Árvore-B dado seu RRN
 *  @param:
 *      FILE* file_btree_r: arquivo de índice Árvore-B
 *      int rrn_b_tree: RRN referente ao nó que será lido de Árvore-B
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      Node*: estutura Node* com os dados lidos
*/
Node* read_node_from_b_tree(FILE *file_btree_r, int rrn_b_tree, int f_type);

/*
 * Busca índice src_id em Árvore-B 
 *  @param:
 *      FILE* file_bin_r: arquivo binário de dados
 *      FILE* file_btree_r: arquivo de índice Árvore-B
 *      int src_id: chave ID que está sendo buscada
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      Header* f_header: cabeçalho de arquivo binário de dados carregado na memória
 *      int f_type: tipo do arquivo de dados
 *  @return: 
 *      long int: referência ao índice buscado
 *      -1: índice não encontrado
*/
long int search_in_page_b_tree(FILE *file_btree_r, Node *cur_node, int src_id, int f_type);

/*
 * Busca índice src_id em Árvore-B 
 *  @param:
 *      FILE* file_bin_r: arquivo binário de dados
 *      FILE* file_btree_r: arquivo de índice Árvore-B
 *      int src_id: chave ID que está sendo buscada
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      Header* f_header: cabeçalho de arquivo binário de dados carregado na memória
 *      int f_type: tipo do arquivo de dados
 *  @return: 
 *      long int: referência ao índice buscado
 *      -1: índice não encontrado
*/
long int search_index_in_b_tree(FILE *file_bin_r, FILE *file_btree_r, int src_id, B_Header *b_header, Header *f_header, int f_type);

/*
 * Checa se nó node se trata de um nó folha
 *  @param:
 *      Node *node: nó analisado
 *  @return:
 *      1: se trata de um nó folha
 *      0: não se trata de um nó folha
*/
int isLeafNode(Node *node);

/* 
 * Escreve um nó no arquivo de índices da Árvore-B
 *  @param:
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      Node *node: nó que será escrito no arquivo
 *      int rrn: RRN do arquivo de índices Árvore-B onde será escrito o novo índice
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
*/
int write_node_in_btree_file(FILE *file_btree_rw, Node *node, int rrn, int f_type);

/*
 * Função de split do nó quando atinge a capacidade máxima. 
 *  @param:
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      Node *cur_node: nó analisado
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      int f_type: tipo do arquivo de dados
 *      Key *promo_key: chave promovida em caso de split
 *      Key *promo_r_child: RRN referente ao filho direito de chave promovida
 *  @return:
 *      0: não ocorre split
 *      1: ocorre split
*/
int split_node(FILE *file_btree_rw, Node *cur_node, B_Header *b_header, int f_type, Key *promo_key, int *promo_r_child);
 
/*
 * Insere um novo id chave na página referenciada por node
 *  @param:
 *      Node *node: nó onde nova chave será inserida
 *      Key new_key: chave a ser inserida
 *  @return:
 *      0: nenhum erro ocorreu
*/
int insert_new_id_in_node(Node *node, Key new_key);
 
/* 
 * Insere um novo RRN de subárvore em um nó especificado por node no índice especificado por index
 *  @param:
 *      Node *node: nó onde nova referência será inserida
 *      int subtree_rrn: nova referência de subárvore a ser inserida
 *      int index: índice onde o novo RRN será inserido
 *  @return:
 *      0: não ocorreu nenhum erro
*/
int insert_subtree_rrn_in_node(Node *node, int subtree_rrn, int index);

/*
 * Executa a rotina de inserção de uma chave
 *  @param:
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      Key new_key: chave a ser inserida em Árvore-B
 *      int cur_rrn_btree: RRN referente ao nó atual de Árvore-B
 *      int f_type: tipo do arquivo de dados
 *      Key *promo_key: chave promovida em caso de split
 *      Key *promo_r_child: RRN referente ao filho direito de chave promovida
 *      Node *cur_node: nó analisado
 *      int i: índice analisado de página referenciada por *node
 *  @return:
 *      0: não ocorre split
 *      1: ocorre split
*/
int insertion_routine(FILE *file_btree_rw, B_Header *b_header, Key new_key, int cur_rrn_btree, int f_type, Key *promo_key, int *promo_r_child, int recursion_counter, Node *cur_node, int i);

/*
 * Procura posição a se inserir nova chave especificada por new_key e chama rotina de inserção
 *  @param:
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      Key new_key: chave a ser inserida em Árvore-B
 *      int cur_rrn_btree: RRN referente ao nó atual de Árvore-B
 *      int f_type: tipo do arquivo de dados
 *      Key *promo_key: chave promovida em caso de split
 *      Key *promo_r_child: RRN referente ao filho direito de chave promovida
 *      int recursion_counter: contador de recursão para controle de nível da árvore
 *  @return:
 *      0: não ocorre split
 *      1: ocorre split
*/
int insert_btree(FILE *file_btree_rw, B_Header *b_header, Key new_key, int cur_rrn_btree, int f_type, Key *promo_key, int *promo_r_child, int recursion_counter);

/*
 * Cria um novo nó raiz quando a árvore está vazia
 *  @param:
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      Key new_key: chave a ser inserida em Árvore-B
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      0: nenhum erro 
*/
int initialize_btree(FILE *file_btree_rw, B_Header *b_header, Key new_key, int f_type);

/*
 * Realiza rotinas de inserção de um novo nó em uma Árvore-B
 *  @param:
 *      FILE* file_btree_rw: arquivo de índices Árvore-B
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *      int id: ID a ser inserido em Árvore-B
 *      int id_ref: referência de ID no arquivo binário de dados 
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
*/
int add_new_node_btree(FILE *file_btree_rw, B_Header *b_header, int id, long int id_ref, int f_type);

/*
 * Escreve arquivo de índices Árvore-B a partir de um arquivo binário de dados
 *  @param:
 *      FILE* file_bin_r: arquivo binário de dados
 *      Header* f_header: cabeçalho de arquivo binário de dados carregado na memória
 *      char *btree_filename: nome do arquivo de índices Árvore-B que será escrito
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      0: nenhum erro ocorreu
*/
int write_btree_file_from_bin(FILE *file_bin_r, Header *f_header, char *btree_filename, int f_type);

/* 
 * Lê status em uma estrutura B_Header e o retorna
 *  @param:
 *      B_Header* b_header: cabeçalho da Árvore-B carregado na memória
 *  @return:
 *      char b_header->status
*/
char get_status_from_btree_header(B_Header *b_header);

/* 
 * Imprime os valores de uma estrutura do tipo Node
 *  @param:
 *      Node *node: nó a cujos dados serão impressos
 *      int f_type: tipo do arquivo de dados
 *  @return:
 *      void
*/
void print_node(Node *node, int f_type);


#endif