# Trabalho Final #1

**Trabalho final 1 da matéria de compiladores desenvolvida em 2025/2.**

Neste trabalho será implementada uma versão completa do compilador (com algumas simplificações), com suporte adicional ao uso de funções. Deve-se considerar como referência, a versão do código entregue na Tarefa 9, ou para o caso do código da tarefa estar inconsistente, utilizar o Código Exemplo 12.

Para esta versão, o compilador deve suportar os principais comandos da linguagem já implementados (declaração de variáveis, condicional, laço, escrita e leitura) e a prototipação de funções, a implementação do(s) código(s) da(s) função(ões) e a chamada(s) à(s) função(ões) prototipada(s). O sistema pode tratar as variáveis das funções e seus parâmetros como variáveis globais (registradas na TS global).

---

## 🛠 Requisito 1

Na prototipação das funções dois módulos principais devem ser ajustados: módulo **"Symbol"** (manipulação das tabelas de símbolos) e módulo **"Synt"** (analisador sintático). Considere as seguintes tarefas a serem cumpridas:

### Módulo “Symbol”:

* Implementar a estrutura de dados da Tabela de Símbolos de Funções (TSF);
* Implementar a função de inserção na TSF de uma nova função;
* Implementar a função de busca por uma função na TSF.

### Módulo “Synt”:

Ajustar a gramática (implementação das suas funções) de modo a atualizar as regras de derivação conforme:

> **Programa** → `Declarações begin Comandos end Func_code`
> **Declarações** → `Declaração | <vazio>`
> **Declaracao** → `{ int | float | string | char } id { DeclaracaoV | DeclaraçãoF }`
> **DeclaraçãoV** → `;`
> **DeclaraçãoF** → `( [{int|float|string|char}id,]* [{int|float|string|char}id] | <vazio> ) ;`

* Ajustar a função correspondente à regra de derivação “program”;
* Ajustar a função correspondente à regra de derivação “declaration”;
* Diferenciar a declaração de variável da declaração de função;
* Ao reconhecer a declaração de função:
* Verificar se a função (id) já está cadastrada na TSF;
* Senão estiver, realiza a inserção da função na TSF.


* Criar e inicializar adequadamente os parâmetros na estrutura da TSF para permitir as verificações nas outras ações que manipulam funções, além de gerar e registrar o respectivo "label" da função;
* Implemente uma função de suporte que imprima a TSF, para verificação dos dados e debugação.

---

## 🛠 Requisito 2

Implemente o reconhecimento de uma chamada de função. Observe especialmente os seguintes aspectos:

* Incluir a regra de derivação “func_call_cmd” nas derivações possíveis de “Statment” (que irá tratar a chamada de uma função);
* Ajustar a gramática para resolver a ambiguidade entre `id = E` e `id ( [id|E]*)` (atribuição de variável x chamada de função);
* Implementar a regra de derivação “func_call_cmd” de tal modo que quando ocorrer uma chamada de uma função no código sejam verificados:
1. O id da função está presente na TS de funções?
2. A quantidade de parâmetros é equivalente ao declarado no protótipo (conforme a TSF)?
3. Para cada parâmetro informado na chamada, eles têm o mesmo id e são do tipo adequado ao prototipado?



> O processo de compilação só continua se todos os testes forem verdadeiros, caso contrário será uma falha de compilação e o erro correspondente deve ser apresentado.

---

## 🛠 Requisito 3

Implemente a última parte do tratamento de funções, reconhecendo o código das funções, com atenção aos principais aspectos inerentes a este processo:

* Na geração de código, encerre o programa ao final do escopo da função principal (código delimitado pelos comandos "begin" e "end" - sem cabeçalho da função principal);
* Adicione e implemente a regra de derivação "Func_code";
* O(s) código(s) da(s) função(ões) deve(m) ocorrer após o código da função principal, bem como o código em linguagem de montagem também deve ser gerado após o fechamento do código principal;
* Garanta que somente as funções prototipadas (cadastradas na TSF) possam ser implementadas;
* Implemente os labels e saltos (jumps) para desvio do fluxo de execução para as funções e respectivamente seu retorno, usando as instruções **"jal"** (para o salto para o escopo da função), e **"jr"** para implementar o retorno da função para o ponto de chamada;
* Para esta versão não precisa ser tratado o retorno a função. Todas terão o perfil de "procedimento", pois as variáveis e parâmetros são todos globais.

---

## Autores
- Anna Bheatryz Martins dos Santos
- Mariana Sanchez Pedroni