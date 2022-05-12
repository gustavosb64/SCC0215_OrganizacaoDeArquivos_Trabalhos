## Anotações 

### Perguntas
* Arquivo tipo 1: 
    * Registro de cabeçalho
        * _status_ assume valor 1 após fechar o arquivo depois de abrí-lo para leitura também?
        * _desC1_: apenas 15 bytes mesmo? Não cabe CODIGO IDENTIFICADOR em 15 bytes
        * _codN_: são os números de fato, ou o char referentes aos números?
    * Registro de dados
        * Se valores nulos não vão ser armazenados, como os dados serão propriamente identificados?

### Arquivo Tipo 1: Registros de tamanho fixo

* **Lixo: '$'**

#### Registro de cabeçalho

* _status_: consistência do arquivo
    * **tam**: char (1 byte)
    * 0 ao abrir o arquivo para escrita
    * 1 ao finalizar o uso
     
* _topo_: RRN de um registro logicamente removido
    * **tam**: int (4 bytes)
    * -1: não tem registros removidos
     
* _descricao_: descrição dos metadados
    * **tam**: char[40] 
    * armazena "LISTAGEM DA FROTA DOS VEICULOS NO BRASIL"
     
* _desC1_: descrição detalhada primeiro campo
    * **tam**: char[15] 
    * armazena "CODIGO IDENTIFICADOR:"
     
* _desC2_: descrição detalhada segundo campo
    * **tam**: char[19]
    * armazena "ANO DE FABRICACAO:"
     
* _desC3_: descrição detalhada terceiro campo
    * **tam**: char[24]
    * armazena "QUANTIDADE DE VEICULOS:"
     
* _desC4_: descrição detalhada quarto campo
    * **tam**: char[8]
    * armazena "ESTADO:"
     
* _codC5_: descrição simplificada quinto campo
    * **tam**: char [1]
    * armazena "0"
     
* _desC5_: descrição detalhada quinto campo
    * **tam**: char[17]
    * armazena "NOME DA CIDADE:"
     
* _codC6_: descrição simplificada sexto campo
    * **tam**: char [1]
    * armazena "1"
     
* _desC6_: descrição detalhada sexto campo
    * **tam**: char[18]
    * armazena "MARCA DO VEICULO:"
     
* _codC7_: descrição simplificada sexto campo
    * **tam**: char [1]
    * armazena "2"
     
* _desC7_: descrição detalhada sétimo campo
    * **tam**: char[19]
    * armazena "MODELO DO VEICULO:"
     
* _proxRRN_: próximo RRN disponível
    * **tam**: int (4 bytes) 
    * iniciado com 0 e incrementado quando necessário
     
* _nroRegRem_: quantidade de registros logicamente removidos
    * **tam**: int (4 bytes) 
    * iniciado com 0 e incrementado quando necessário

#### Registro de dados

* Tamanho fixo: 97 bytes
* Campos de tamanho fixo e campos de tamanho variável
* **Método indicador de tamanho**

* **Tamanho fixo**
    * _id_: int
    * _ano_: int
    * _qtt_: int
    * _sigla_: char[2]
    * _removido_: char[1] 
    * _prox_: int
    * _tamCidade_: int
    * _tamMarca_: int
    * _tamModelo_: int
* **Tamanho variável**
    * _cidade_
    * _marca_
    * _modelo_
