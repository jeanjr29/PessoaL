#include <stdio.h>   // Biblioteca padrão para entrada e saída de dados.
#include <stdlib.h>  // Biblioteca padrão para funções de propósito geral, como alocação de memória.
#include <string.h>  // Biblioteca padrão para manipulação de strings.
#include <locale.h>  // Biblioteca padrão para definição de idioma
#include <windows.h> // Inclui a biblioteca Windows.h, necessária para usar funções específicas do Windows, como SetConsoleOutputCP
#include <ctype.h>   // Inclui a biblioteca ctype.h, que fornece funções para manipulação de caracteres, como isdigit para verificar se um caractere é um dígito

// Definição da struct do registro para Login
typedef struct
{
    char cpf[12]; // Vetor do tipo char (string) para armazenar CPF
    char senha[10]; // Vetor do tipo char (string) para armazenar senha
    char nome[50]; // Vetor do tipo char (string) para armazenar nome
    int privilegio; // Variável do tipo inteiro para armazenar nível de privilégio

} membroAcademico;

// Definição da struct dos Estudantes
typedef struct
{
    size_t tamNome; // Usar size_t para tamanhos, utilizando para tamanhos variáveis
    char *nome; // Ponteiro para o nome
    float p1; // Variável do tipo float para guardar a nota P1
    float p2; // Variável do tipo float para guardar a nota P2
    float media; // Variável do tipo float para guardar a média das notas
    int faltas; // Variável do tipo inteiro para armazenar a quantidade de faltas
    char situ[10]; // Vetor do tipo char (string) para armazenar a situação do aluno na matéria (Aprovado ou Reprovado)

} Estudante;

int userLoggedOut = 0; // Variável global para rastrear logout

// Função para ler um registro do tipo Estudante no arquivo binário
int LerArquivo(FILE *file, Estudante *aluno)
{
    // Lê o tamanho do nome do estudante do arquivo.
    if (fread(&aluno->tamNome, sizeof(size_t), 1, file) != 1)
    {
        return 0; // Retorna 0 se a leitura falhar
    }

    // Aloca memória para armazenar o nome do estudante, adicionando espaço para o terminador nulo.
    aluno->nome = (char *)malloc(aluno->tamNome + 1);
    if (aluno->nome == NULL)
    {
        return 0; // Retorna 0 se a alocação de memória falhar
    }

    // Lê o nome do estudante do arquivo.
    if (fread(aluno->nome, sizeof(char), aluno->tamNome, file) != aluno->tamNome)
    {
        free(aluno->nome); // Libera a memória alocada se a leitura falhar
        return 0;          // Retorna 0 se a leitura falhar
    }
    aluno->nome[aluno->tamNome] = '\0'; // Adiciona o terminador nulo ao final do nome para indicar o final da string

    // Lê as notas p1 e p2, média, faltas e a situação do estudante do arquivo.
    if (fread(&aluno->p1, sizeof(float), 1, file) != 1 ||
        fread(&aluno->p2, sizeof(float), 1, file) != 1 ||
        fread(&aluno->media, sizeof(float), 1, file) != 1 ||
        fread(&aluno->faltas, sizeof(int), 1, file) != 1 ||
        fread(aluno->situ, sizeof(char), 10, file) != 10)
    {
        free(aluno->nome); // Libera a memória alocada se a leitura falhar
        return 0;          // Retorna 0 se a leitura falhar
    }
    return 1; // Retorna 1 se a leitura for bem-sucedida
}

// Função para ler um registro do tipo membroAcademico no arquivo binário
int LerUsuario(FILE *file, membroAcademico *usuario)
{
    // Lê o CPF do usuário do arquivo. Espera-se que o CPF tenha exatamente 12 caracteres.
    if (fread(usuario->cpf, sizeof(char), 12, file) != 12)
    {
        return 0; // Retorna 0 se a leitura falhar
    }

    // Lê a senha do usuário do arquivo. Espera-se que a senha tenha exatamente 10 caracteres.
    if (fread(usuario->senha, sizeof(char), 10, file) != 10)
    {
        return 0; // Retorna 0 se a leitura falhar
    }

    // Lê o nome do usuário do arquivo. Espera-se que o nome tenha exatamente 50 caracteres.
    if (fread(usuario->nome, sizeof(char), 50, file) != 50)
    {
        return 0; // Retorna 0 se a leitura falhar
    }

    // Lê o privilégio do usuário do arquivo. Privilégio é um inteiro que representa o nível de acesso do usuário.
    if (fread(&usuario->privilegio, sizeof(int), 1, file) != 1)
    {
        return 0; // Retorna 0 se a leitura falhar
    }

    return 1; // Retorna 1 se a leitura for bem-sucedida
}

// Função para pular o nome da Matéria e do Professor escritos no arquivo binário
void PularMatProf(FILE *file)
{
    // Declara variáveis para armazenar temporariamente os dados
    char nomeMateria[100]; // Buffer para armazenar o nome da matéria
    char profTemp[50];     // Buffer para armazenar o nome do professor
    int numAlunos;         // Variável para armazenar o número de alunos

    // Lê o nome da matéria do arquivo e descarta
    fread(nomeMateria, sizeof(char), 100, file);

    // Lê o nome do professor do arquivo e descarta
    fread(profTemp, sizeof(char), 50, file);

    // Lê o número de alunos do arquivo e descarta
    fread(&numAlunos, sizeof(int), 1, file);
}

// Função para pegar um registro específico do arquivo binário com base no nome e retornar os dados a partir de P1
Estudante *EncontrarDadoPorNome(const char *nomeArquivo, const char *nome)
{

    // Abre o arquivo binário para leitura (r de read e b de binário)
    FILE *file = fopen(nomeArquivo, "rb");

    // Verifica se o arquivo existe ou se tem permissão para acessá-lo
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo %s", nomeArquivo); // Mensagem de erro
        return NULL;                       // Retorna a função com o valor nulo
    }

    PularMatProf(file);

    Estudante temp;           // Variável do tipo struct Estudante para comparar na função
    Estudante *result = NULL; // Ponteiro em que será guardado o resultado da função

    // Enquanto a função LerArquivo retornar 1
    while (LerArquivo(file, &temp))
    {

        // Compara o valor da linha no arquivo binário com o nome que queremos encontrar
        if (strcmp(temp.nome, nome) == 0)
        {

            // Result recebe o endereço de blocos consecutivos na memória que malloc (memory allocation) retorna, sendo convertido para a struct do tipo Estudante
            result = (Estudante *)malloc(sizeof(Estudante));

            // Se o result for diferente de nulo
            if (result != NULL)
            {
                *result = temp; // Copiar o registro encontrado
            }
            break; // Sai do if e consequentemente sai do laço while
        }
        free(temp.nome); // Liberar o nome do registro temporário se não for o desejado
    }

    fclose(file);  // Fecha o arquivo
    return result; // Retorna dessa função o valor de result
}

// Função para escrever um registro do tipo Estudante no arquivo binário
void EscreverEstudante(FILE *file, Estudante *aluno)
{
    // Escreve o tamanho do nome do estudante no arquivo
    fwrite(&aluno->tamNome, sizeof(size_t), 1, file);
    
    // Escreve o nome do estudante no arquivo
    fwrite(aluno->nome, sizeof(char), aluno->tamNome, file);
    
    // Escreve a nota P1 do estudante no arquivo
    fwrite(&aluno->p1, sizeof(float), 1, file);
    
    // Escreve a nota P2 do estudante no arquivo
    fwrite(&aluno->p2, sizeof(float), 1, file);
    
    // Escreve a média do estudante no arquivo
    fwrite(&aluno->media, sizeof(float), 1, file);
    
    // Escreve o número de faltas do estudante no arquivo
    fwrite(&aluno->faltas, sizeof(int), 1, file);
    
    // Escreve a situação do estudante no arquivo (tamanho fixo de 10 caracteres)
    fwrite(aluno->situ, sizeof(char), 10, file);
}

// Função para escrever um registro do tipo membroAcademico no arquivo binário
void EscreverUsuario(FILE *file, membroAcademico *usuario)
{
    // Escreve o CPF do usuário no arquivo (tamanho fixo de 12 caracteres)
    fwrite(usuario->cpf, sizeof(char), 12, file);
    
    // Escreve a senha do usuário no arquivo (tamanho fixo de 10 caracteres)
    fwrite(usuario->senha, sizeof(char), 10, file);
    
    // Escreve o nome do usuário no arquivo (tamanho fixo de 50 caracteres)
    fwrite(usuario->nome, sizeof(char), 50, file);
    
    // Escreve o privilégio do usuário no arquivo (um inteiro)
    fwrite(&usuario->privilegio, sizeof(int), 1, file);
}

// Função para verificar se um arquivo de matéria existe
int MateriaExiste(const char *nomeMateria)
{
    // Declaração de um array de caracteres para armazenar o nome do arquivo
    char nomeArquivo[105];

    // Formata o nome do arquivo acrescentando ".bin" ao nome da matéria e armazena em nomeArquivo
    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.bin", nomeMateria);
    
    // Tenta abrir o arquivo no modo de leitura binária
    FILE *file = fopen(nomeArquivo, "rb");
    
    // Se o arquivo foi aberto com sucesso
    if (file)
    {
        // Fecha o arquivo
        fclose(file);
        
        // Retorna 1 indicando que o arquivo existe
        return 1;
    }

    // Se o arquivo não pôde ser aberto, retorna 0 indicando que o arquivo não existe
    return 0;
}

// Função para verificar se um professor específico existe no arquivo "login.bin"
int ProfessorExiste(const char *nomeProf)
{
    // Tenta abrir o arquivo "login.bin" no modo de leitura binária
    FILE *file = fopen("login.bin", "rb");

    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        // Retorna 0 se o arquivo não puder ser aberto
        return 0;
    }

    // Declara uma variável do tipo membroAcademico para armazenar os dados lidos do arquivo
    membroAcademico usuario;

    // Enquanto a função LerUsuario retornar 1 (indicação de que a leitura foi bem-sucedida)
    while (LerUsuario(file, &usuario))
    {
        // Compara o nome do usuário lido com o nome do professor que queremos encontrar e verifica se o privilégio é igual a 2 (indicando que é um professor)
        if (strcmp(usuario.nome, nomeProf) == 0 && usuario.privilegio == 2)
        {
            // Fecha o arquivo
            fclose(file);
            
            // Retorna 1 indicando que o professor foi encontrado
            return 1;
        }
    }

    // Fecha o arquivo
    fclose(file);
    
    // Retorna 0 indicando que o professor não foi encontrado
    return 0;
}

// Função para verificar se um professor específico existe no arquivo "login.bin"
void ListarProfessores()
{
    // Tenta abrir o arquivo "login.bin" no modo de leitura binária
    FILE *file = fopen("login.bin", "rb");

    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        // Retorna 0 se o arquivo não puder ser aberto
        return;
    }

    // Declara uma variável do tipo membroAcademico para armazenar os dados lidos do arquivo
    membroAcademico usuario;

    // Cabeçalho da tabela de usuários
    printf("\nLista de Professores:\n");
    printf("CPF          | Nome                      | Privilégio |\n");
    printf("---------------------------------------------------------\n");

    // Enquanto a função LerUsuario retornar 1 (indicação de que a leitura foi bem-sucedida)
    while (LerUsuario(file, &usuario))
    {
        // Compara o nome do usuário lido com o nome do professor que queremos encontrar e verifica se o privilégio é igual a 2 (indicando que é um professor)
        if (usuario.privilegio == 2)
        {
            // Exibe os dados do usuário formatados
            printf("%-12s | %-25s | %-10d |\n", usuario.cpf, usuario.nome, usuario.privilegio);
            
        }
    }

    // Fecha o arquivo
    fclose(file);
}

// Função para verificar se o professor está vinculado à matéria
int ProfessorExisteNaMateria(const char *nomeMateria, const char *nomeProfessor)
{
    // Declaração do nome do arquivo a partir do nome da matéria
    char nomeArquivo[105];
    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.bin", nomeMateria);

    // Abre o arquivo em modo de leitura binária
    FILE *file = fopen(nomeArquivo, "rb");
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    char buffer[50];
    // Pula os primeiros 100 bytes (nome da matéria) no arquivo
    fseek(file, 100, SEEK_CUR);
    // Lê os próximos 50 bytes (nome do professor) do arquivo
    fread(buffer, sizeof(char), 50, file);
    // Remove qualquer caractere de nova linha do final da string lida
    buffer[strcspn(buffer, "\n")] = '\0';

    // Fecha o arquivo
    fclose(file);
    // Compara o nome do professor lido do arquivo com o nome do professor fornecido
    return strcmp(buffer, nomeProfessor) == 0;
}

// Função para verificar se o aluno está matriculado na matéria
int AlunoExisteNaMateria(const char *nomeMateria, const char *nomeAluno)
{
    // Declaração do nome do arquivo a partir do nome da matéria
    char nomeArquivo[105];
    // Variáveis temporárias para armazenar o nome da matéria, professor e número de alunos
    char materiaTemp[100];
    char profTemp[50];
    int numAlunos;

    // Formatação do nome do arquivo a partir do nome da matéria
    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.bin", nomeMateria);

    // Abre o arquivo em modo de leitura binária
    FILE *file = fopen(nomeArquivo, "rb");
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        printf("Erro: Arquivo da matéria não encontrado.\n");
        return 0;
    }

    // Lê o nome da matéria, nome do professor e o número de alunos do arquivo
    fread(materiaTemp, sizeof(char), 100, file);
    fread(profTemp, sizeof(char), 50, file);
    fread(&numAlunos, sizeof(int), 1, file);

    // Itera sobre os alunos para verificar se o aluno procurado está na matéria
    int i;
	for ( i = 0; i < numAlunos; i++)
    {
        Estudante temp;
        // Lê os dados de um aluno do arquivo e verifica se o nome coincide com o nome procurado
        if (LerArquivo(file, &temp) && strcmp(temp.nome, nomeAluno) == 0)
        {
            // Libera a memória alocada para o nome do aluno
            free(temp.nome);
            // Fecha o arquivo e retorna 1, indicando que o aluno está na matéria
            fclose(file);
            return 1;
        }
        // Libera a memória alocada para o nome do aluno
        free(temp.nome);
    }

    // Fecha o arquivo e retorna 0, indicando que o aluno não está na matéria
    fclose(file);
    return 0;
}

// Função para formatar o arquivo com alguns usuários
void FormatarArquivoLogin(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    membroAcademico usuarios[3] = {
        {"12345678901", "senha1", "Bruno", 1},
        {"12345678902", "senha2", "Luciano", 2},
        {"12345678903", "senha3", "Joao", 3}};

    int i;
	for ( i = 0; i < 3; i++)
    {
        EscreverUsuario(file, &usuarios[i]);
    }

    fclose(file);
}

// Função para validar o CPF
int ValidarCPF(const char *cpf) {

    // Verifica se o CPF tem exatamente 11 caracteres
    if (strlen(cpf) != 11) {
        return 0;
    }
    // Verifica se todos os caracteres do CPF são dígitos
    int i;
	for ( i = 0; i < 11; i++) {
        if (!isdigit(cpf[i])) {
            return 0;
        }
    }
    return 1;
}

// Função para verificar se o CPF corresponde à senha e retornar o usuário encontrado
int Login(const char *nomeArquivo, const char *cpf, const char *senha, membroAcademico *usuarioEncontrado)
{
    // Tenta abrir o arquivo nomeArquivo no modo de leitura binária
    FILE *file = fopen(nomeArquivo, "rb");

    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        perror("Erro ao abrir o arquivo");
        // Retorna 0 se o arquivo não puder ser aberto
        return 0;
    }

    // Declara uma variável do tipo membroAcademico para armazenar os dados lidos do arquivo
    membroAcademico usuario;

    // Enquanto a função LerUsuario retornar 1 (indicação de que a leitura foi bem-sucedida)
    while (LerUsuario(file, &usuario))
    {
        // Compara o CPF e a senha do usuário lido com o CPF e a senha fornecidos
        if (strcmp(usuario.cpf, cpf) == 0 && strcmp(usuario.senha, senha) == 0)
        {
            // Copia os dados do usuário encontrado para a variável usuarioEncontrado
            *usuarioEncontrado = usuario;
            // Fecha o arquivo
            fclose(file);
            // Retorna 1 indicando que o CPF e a senha correspondem
            return 1;
        }
    }

    // Fecha o arquivo
    fclose(file);
    // Retorna 0 indicando que o CPF e a senha não correspondem
    return 0;
}

// Função para encontrar as notas de um aluno específico na matéria
void MostrarNotas(char *materia, char *nome)
{

    // Atribuindo a uma variável o valor da chamada da função EncontrarDadoPorNome
    Estudante *aluno = EncontrarDadoPorNome(materia, nome);

    // Verifica se foi retornado algum valor pela função
    if (aluno != NULL)
    {

        // Imprime na tela os dados encontrados a partir da P1
        printf("Dados encontrados: P1 = %.2f, P2 = %.2f, Média = %.2f\n", aluno->p1, aluno->p2, aluno->media);
        free(aluno->nome); // Libera a memória alocada para o nome
        free(aluno);       // Liberar a memória alocada para toda a variável aluno
    }
    else
    {

        // Imprime que o nome que queremos não foi encontrado
        printf("Nome '%s' não encontrado no arquivo.\n", nome);
    }
}

// Função para encontrar as faltas de um aluno específico na matéria
void MostrarFaltas(char *materia, char *nome)
{

    // Atribuindo a uma variável o valor da chamada da função EncontrarDadoPorNome
    Estudante *aluno = EncontrarDadoPorNome(materia, nome);

    // Verifica se foi retornado algum valor pela função
    if (aluno != NULL)
    {

        // Imprime na tela os dados encontrados a partir da P1
        printf("Dados encontrados: Faltas = %i\n", aluno->faltas);
        free(aluno->nome); // Libera a memória alocada para o nome
        free(aluno);       // Liberar a memória alocada para toda a variável aluno
    }
    else
    {

        // Imprime que o nome que queremos não foi encontrado
        printf("Nome '%s' não encontrado no arquivo.\n", nome);
    }
}

// Função para listar todos os alunos registrados na matéria
void MostrarTodosOsAlunos(const char *nomeArquivo)
{
    // Tenta abrir o arquivo nomeArquivo no modo de leitura binária
    FILE *file = fopen(nomeArquivo, "rb");

    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        // Retorna sem fazer nada se o arquivo não puder ser aberto
        return;
    }

    // Pula os dados da matéria e do professor no início do arquivo
    PularMatProf(file);

    // Declara uma variável do tipo Estudante para armazenar os dados lidos do arquivo
    Estudante aluno;

    // Exibe o cabeçalho da tabela de alunos registrados
    printf("Tabela de alunos registrados:\n");
    printf("-------------------------------\n");
    printf("| Nome                          |\n");
    printf("-------------------------------\n");

    // Enquanto a função LerArquivo retornar 1 (indicação de que a leitura foi bem-sucedida)
    while (LerArquivo(file, &aluno))
    {
        // Exibe o nome do aluno formatado com um espaço de 30 caracteres
        printf("| %-30s |\n", aluno.nome);
        // Libera a memória alocada para o nome do aluno
        free(aluno.nome);
    }

    // Exibe o rodapé da tabela
    printf("-------------------------------\n");

    // Fecha o arquivo
    fclose(file);
}

// Função para atribuir notas a um aluno
void AtribuirNotas(char *materia)
{
    char nome[50];       // Array para armazenar o nome do aluno
    int escolha;         // Variável para armazenar a escolha do usuário (P1 ou P2)
    float novaNota;      // Variável para armazenar a nova nota
    int encontrado;      // Flag para verificar se o aluno foi encontrado

    // Abre o arquivo da matéria no modo de leitura e escrita binária
    FILE *file = fopen(materia, "rb+");
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        printf("Erro ao abrir o arquivo %s\n", materia);
        return; // Retorna sem fazer nada se o arquivo não puder ser aberto
    }

    // Loop para solicitar o nome do aluno até que um nome válido seja inserido
    while (1)
    {
        encontrado = 0; // Inicializa a flag para verificar se o aluno foi encontrado

        // Mostra a lista de todos os alunos registrados na matéria
        MostrarTodosOsAlunos(materia);

        // Solicita ao usuário o nome do aluno
        printf("Digite o nome do aluno: ");
        scanf("%s", nome);

        // Move o ponteiro do arquivo de volta ao início após a lista de alunos
        fseek(file, 0, SEEK_SET);
        PularMatProf(file);

        Estudante aluno; // Declara uma variável do tipo Estudante para armazenar os dados lidos do arquivo

        // Enquanto a função LerArquivo retornar 1 (indicação de que a leitura foi bem-sucedida)
        while (LerArquivo(file, &aluno))
        {
            // Compara o nome lido com o nome fornecido pelo usuário
            if (strcmp(aluno.nome, nome) == 0)
            {
                encontrado = 1; // Marca que o aluno foi encontrado

                // Exibe as notas atuais do aluno
                printf("Notas atuais - P1: %.2f, P2: %.2f, Média: %.2f\n", aluno.p1, aluno.p2, aluno.media);

                // Loop para solicitar e validar a escolha do usuário (P1 ou P2)
                while (1)
                {
                    printf("Escolha qual avaliação deseja lançar/alterar a nota:\n1 - P1\n2 - P2\n");
                    if (scanf("%d", &escolha) == 1 && (escolha == 1 || escolha == 2))
                    {
                        break; // Sai do loop se a entrada for válida
                    }
                    else
                    {
                        // Informa o usuário que a escolha inserida é inválida
                        printf("Opção inválida! Escolha 1 para P1 ou 2 para P2.\n");
                        // Limpa o buffer de entrada para remover a entrada inválida
                        while (getchar() != '\n');
                    }
                }

                // Loop para solicitar e validar a nova nota
                while (1)
                {
                    printf("Digite a nova nota para P%d (0 a 10): ", escolha);
                    if (scanf("%f", &novaNota) == 1 && novaNota >= 0 && novaNota <= 10)
                    {
                        break; // Sai do loop se a entrada for válida
                    }
                    else
                    {
                        // Informa o usuário que a nota inserida é inválida
                        printf("Nota inválida! Deve ser um valor entre 0 e 10.\n");
                        // Limpa o buffer de entrada para remover a entrada inválida
                        while (getchar() != '\n');
                    }
                }

                // Atualiza a nota do aluno
                if (escolha == 1)
                {
                    aluno.p1 = novaNota;
                }
                else if (escolha == 2)
                {
                    aluno.p2 = novaNota;
                }

                // Calcula a nova média do aluno
                aluno.media = (aluno.p1 + aluno.p2) / 2.0;
                // Define a situação do aluno (Aprovado ou Reprovado) com base na média
                strcpy(aluno.situ, aluno.media >= 6.0 ? "Aprovado" : "Reprovado");

                // Move o ponteiro do arquivo de volta para o início do registro do aluno
                fseek(file, -((long)(sizeof(size_t) + aluno.tamNome + sizeof(float) * 3 + sizeof(int) + 10)), SEEK_CUR);
                // Reescreve o registro atualizado do aluno no arquivo
                EscreverEstudante(file, &aluno);
                printf("Notas atualizadas com sucesso!\n");

                free(aluno.nome);  // Libera a memória alocada para o nome do aluno
                break;             // Sai do loop de leitura do arquivo
            }
            free(aluno.nome);  // Libera a memória alocada para o nome do aluno se não for o desejado
        }

        // Se o aluno for encontrado, sai do loop externo
        if (encontrado)
        {
            break;
        }
        else
        {
            printf("Aluno '%s' não encontrado. Por favor, tente novamente.\n", nome);
            system("Pause");
            system("cls");
        }
    }

    fclose(file);  // Fecha o arquivo
}

// Função para realizar chamada
void RealizarChamada(char *materia)
{
    int presente; // Variável para armazenar se o aluno está presente

    // Abre o arquivo da matéria no modo de leitura e escrita binária
    FILE *file = fopen(materia, "rb+");
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        perror("Erro ao abrir o arquivo");
        return; // Retorna sem fazer nada se o arquivo não puder ser aberto
    }

    // Pula os dados da matéria e do professor no início do arquivo
    PularMatProf(file);

    Estudante aluno; // Declara uma variável do tipo Estudante para armazenar os dados lidos do arquivo
    long pos; // Variável para armazenar a posição atual no arquivo

    // Enquanto a leitura do tamanho do nome do aluno for bem-sucedida
    while (fread(&aluno.tamNome, sizeof(size_t), 1, file) == 1)
    {
        // Aloca memória para armazenar o nome do aluno
        aluno.nome = (char *)malloc(aluno.tamNome + 1);
        // Se a leitura do nome do aluno não for bem-sucedida, libera a memória e sai do loop
        if (fread(aluno.nome, sizeof(char), aluno.tamNome, file) != aluno.tamNome)
        {
            free(aluno.nome);
            break;
        }
        aluno.nome[aluno.tamNome] = '\0'; // Adiciona o terminador nulo ao final do nome do aluno
        // Se a leitura dos demais campos do aluno não for bem-sucedida, libera a memória e sai do loop
        if (fread(&aluno.p1, sizeof(float), 1, file) != 1 ||
            fread(&aluno.p2, sizeof(float), 1, file) != 1 ||
            fread(&aluno.media, sizeof(float), 1, file) != 1 ||
            fread(&aluno.faltas, sizeof(int), 1, file) != 1 ||
            fread(aluno.situ, sizeof(char), 10, file) != 10)
        {
            free(aluno.nome);
            break;
        }

        // Exibe o nome e as faltas atuais do aluno
        printf("Nome: %s, Faltas: %d\n", aluno.nome, aluno.faltas);
        
        // Loop para garantir que apenas 1 ou 0 sejam aceitos
        while (1)
        {
            // Pergunta se o aluno está presente
            printf("O aluno está presente? (1 para sim, 0 para não): ");
            if (scanf("%d", &presente) == 1 && (presente == 0 || presente == 1))
            {
                break; // Sai do loop se a entrada for válida
            }
            else
            {
                printf("Entrada inválida. Por favor, insira 1 para sim ou 0 para não.\n");
                // Limpa o buffer de entrada para remover a entrada inválida
                while (getchar() != '\n');
            }
        }

        // Se o aluno não estiver presente, incrementa o número de faltas
        if (!presente)
        {
            aluno.faltas += 1;
        }

        // Armazena a posição atual do ponteiro do arquivo
        pos = ftell(file);
        // Move o ponteiro do arquivo de volta para o início do registro do aluno
        fseek(file, pos - ((long)(sizeof(size_t) + aluno.tamNome + sizeof(float) * 3 + sizeof(int) + 10)), SEEK_SET);
        // Reescreve o registro atualizado do aluno no arquivo
        EscreverEstudante(file, &aluno);
        // Move o ponteiro do arquivo de volta para a posição original
        fseek(file, pos, SEEK_SET);

        free(aluno.nome); // Libera a memória alocada para o nome do aluno
    }

    fclose(file); // Fecha o arquivo
    printf("Chamada realizada com sucesso!\n"); // Exibe uma mensagem de sucesso
    system("pause"); // Pausa o sistema (Windows)
}

// Função para fechar matéria e exibir relatório
void FechamentoMateria(char *materia)
{
    int opcao;           // Variável para armazenar a opção do usuário
    char nomeAluno[50];  // Variável para armazenar o nome do aluno
    FILE *file;          // Ponteiro para o arquivo
    Estudante aluno;     // Variável do tipo Estudante para armazenar os dados dos alunos

    // Loop para solicitar e validar a opção do usuário
    while (1)
    {
        // Exibe as opções para o usuário
        printf("Deseja exibir o relatório por:\n1 - Aluno\n2 - Disciplina\n");
        
        // Verifica se a entrada é um número válido
        if (scanf("%d", &opcao) == 1 && (opcao == 1 || opcao == 2))
        {
            break; // Sai do loop se a entrada for válida
        }
        else
        {
            // Informa o usuário que a opção inserida é inválida
            printf("Opção inválida! Escolha 1 para Aluno ou 2 para Disciplina.\n");
            system("pause");
            // Limpa o buffer de entrada para remover a entrada inválida
            while (getchar() != '\n');
            system("cls");
        }
    }

    // Se a opção for 1, exibir relatório por aluno
    if (opcao == 1)
    {
        // Loop para solicitar e validar o nome do aluno
        while (1)
        {
            // Inicializa a flag de encontrado
            int encontrado = 0;

            // Mostra a lista de todos os alunos registrados na matéria
            MostrarTodosOsAlunos(materia);
            // Solicita o nome do aluno
            printf("Digite o nome do aluno: ");
            // Lê o nome do aluno
            scanf("%49s", nomeAluno);

            // Abre o arquivo da matéria para leitura binária
            file = fopen(materia, "rb");
            // Verifica se o arquivo foi aberto com sucesso
            if (file == NULL)
            {
                perror("Erro ao abrir o arquivo");
                return;
            }

            // Pula as informações da matéria e do professor no arquivo
            PularMatProf(file);

            // Loop para ler todos os registros de alunos do arquivo
            while (LerArquivo(file, &aluno))
            {
                // Se o nome do aluno lido for igual ao nome fornecido
                if (strcmp(aluno.nome, nomeAluno) == 0)
                {
                    encontrado = 1; // Marca que o aluno foi encontrado
                    // Exibe as informações do aluno
                    printf("\nAluno encontrado:\n");
                    printf("Nome: %s\n", aluno.nome);
                    printf("P1: %.2f\n", aluno.p1);
                    printf("P2: %.2f\n", aluno.p2);
                    printf("Media: %.2f\n", aluno.media);
                    printf("Faltas: %d\n", aluno.faltas);
                    printf("Situação: %s\n", aluno.situ);
                    free(aluno.nome); // Libera a memória alocada para o nome do aluno
                    break; // Sai do loop
                }
                free(aluno.nome); // Libera a memória alocada para o nome do aluno
            }

            // Fecha o arquivo
            fclose(file);

            // Se o aluno foi encontrado, sai do loop
            if (encontrado)
            {
                break;
            }
            else
            {
                printf("Aluno '%s' não encontrado. Por favor, tente novamente.\n", nomeAluno);
                system("pause");
                // Limpa o buffer de entrada para remover a entrada inválida
                while (getchar() != '\n');
                system("cls");
            }
        }
    }
    // Se a opção for 2, exibir relatório por disciplina
    else if (opcao == 2)
    {
        // Abre o arquivo da matéria para leitura binária
        file = fopen(materia, "rb");
        // Verifica se o arquivo foi aberto com sucesso
        if (file == NULL)
        {
            perror("Erro ao abrir o arquivo");
            return;
        }

        // Pula as informações da matéria e do professor no arquivo
        PularMatProf(file);

        // Exibe o cabeçalho do relatório por disciplina
        printf("Relatório por disciplina:\n");

        // Loop para ler todos os registros de alunos do arquivo
        while (LerArquivo(file, &aluno))
        {
            // Exibe as informações de cada aluno
            printf("\nNome: %s\n", aluno.nome);
            printf("P1: %.2f\n", aluno.p1);
            printf("P2: %.2f\n", aluno.p2);
            printf("Media: %.2f\n", aluno.media);
            printf("Faltas: %d\n", aluno.faltas);
            printf("Situação: %s\n", aluno.situ);
            free(aluno.nome); // Libera a memória alocada para o nome do aluno
        }

        // Fecha o arquivo
        fclose(file);
    }
}

// Função para listar todos os usuários do sistema
void ListarUsuarios(const char *nomeArquivo)
{
    // Abre o arquivo binário para leitura
    FILE *file = fopen(nomeArquivo, "rb");
    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Variável do tipo membroAcademico para armazenar os dados dos usuários
    membroAcademico usuario;
    
    // Cabeçalho da tabela de usuários
    printf("\nLista de Usuários:\n");
    printf("CPF          | Nome                       | Privilégio |\n");
    printf("---------------------------------------------------------\n");
    
    // Loop para ler todos os registros de usuários do arquivo
    while (LerUsuario(file, &usuario))
    {
        // Exibe os dados do usuário formatados
        printf("%-12s | %-25s | %-10d |\n", usuario.cpf, usuario.nome, usuario.privilegio);
    }

    // Fecha o arquivo
    fclose(file);
}

// Função para cadastrar um novo usuário
void CadastrarUsuario(const char *nomeArquivo)
{
    // Abre o arquivo binário para adição (append)
    FILE *file = fopen(nomeArquivo, "ab");
    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Declaração de uma variável para armazenar os dados do novo usuário
    membroAcademico novoUsuario;
    

    while (1)
    {
        printf("\n\nDigite o CPF: ");
        // Lê o CPF do usuário, com no máximo 11 caracteres
        scanf("%11s", novoUsuario.cpf);
        // Valida o CPF
        if (ValidarCPF(novoUsuario.cpf)) {
            break;
        }
        else
        {
            printf("CPF inválido! Certifique-se de que ele contenha apenas números e 11 dígitos.\n");
            system("pause");
            // Limpa o buffer de entrada para remover a entrada inválida
            while (getchar() != '\n');
            system("cls");
        }
    }

    printf("Digite a senha: ");
    // Lê a senha do usuário, com no máximo 9 caracteres
    scanf("%9s", novoUsuario.senha);
    printf("Digite o nome: ");
    // Lê o nome do usuário, com no máximo 49 caracteres
    scanf("%49s", novoUsuario.nome);

    // Limpa o buffer de entrada após ler o nome
    while(getchar() != '\n'); 

    // Buffer para ler a entrada do privilégio
    char buffer[10]; 
    // Flag para indicar se o privilégio é válido
    int privilegioValido = 0; 

    // Loop para garantir que o privilégio inserido seja válido
    do
    {
        printf("Digite o privilégio (1 - Admin, 2 - Professor, 3 - Aluno): ");
        // Lê a entrada como string
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            // Tenta converter a string para um inteiro
            char *endptr;
            int temp = strtol(buffer, &endptr, 10);
            // Verifica se a conversão foi bem-sucedida e se o valor está no intervalo desejado
            if (endptr != buffer && *endptr == '\n' && temp >= 1 && temp <= 3)
            {
                novoUsuario.privilegio = temp;
                privilegioValido = 1;
            }
            else
            {
                printf("Valor inválido. Por favor, insira 1, 2 ou 3.\n");
            }
        }
    } while (!privilegioValido);

    // Escreve o novo usuário no arquivo
    EscreverUsuario(file, &novoUsuario);
    // Fecha o arquivo
    fclose(file);
    printf("Usuário cadastrado com sucesso!\n");

    // Chama a função para listar todos os usuários
    ListarUsuarios(nomeArquivo);
}

// Função para alterar um usuário existente
void AlterarUsuario(const char *nomeArquivo, const char *cpfAtual)
{
    // Abre o arquivo binário para leitura e escrita
    FILE *file = fopen(nomeArquivo, "rb+");
    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char cpf[12];

    while (1)
    {
        // Lista todos os usuários existentes
        ListarUsuarios(nomeArquivo);

        printf("Digite o CPF do usuário a ser alterado: ");
        // Lê o CPF do usuário que será alterado, com no máximo 11 caracteres
        scanf("%11s", cpf);
        // Valida o CPF
        if (ValidarCPF(cpf)) {
            break;
        }
        else
        {
            printf("CPF inválido! Certifique-se de que ele contenha apenas números e 11 dígitos.\n");
            system("pause");
            // Limpa o buffer de entrada para remover a entrada inválida
            while (getchar() != '\n');
            system("cls");
        }
    }

    

    // Variável para armazenar os dados do usuário
    membroAcademico usuario;
    int encontrado = 0;

    // Percorre o arquivo para encontrar o usuário pelo CPF
    while (LerUsuario(file, &usuario))
    {
        if (strcmp(usuario.cpf, cpf) == 0)
        {
            encontrado = 1;
            printf("Usuário encontrado.\n");
            printf("Digite a nova senha: ");
            // Lê a nova senha do usuário
            scanf("%9s", usuario.senha);
            printf("Digite o novo nome: ");
            // Lê o novo nome do usuário
            scanf("%49s", usuario.nome);

            // Limpa o buffer de entrada após ler o nome
            while (getchar() != '\n');

            // Buffer para ler a entrada do privilégio
            char buffer[10]; 
            // Flag para indicar se o privilégio é válido
            int privilegioValido = 0; 

            // Loop para garantir que o privilégio inserido seja válido
            do
            {
                printf("Digite o privilégio (1 - Admin, 2 - Professor, 3 - Aluno): ");
                // Lê a entrada como string
                if (fgets(buffer, sizeof(buffer), stdin) != NULL)
                {
                    // Tenta converter a string para um inteiro
                    char *endptr;
                    int temp = strtol(buffer, &endptr, 10);
                    // Verifica se a conversão foi bem-sucedida e se o valor está no intervalo desejado
                    if (endptr != buffer && (*endptr == '\n' || *endptr == '\0') && temp >= 1 && temp <= 3)
                    {
                        usuario.privilegio = temp;
                        privilegioValido = 1;
                    }
                    else
                    {
                        printf("Valor inválido. Por favor, insira 1, 2 ou 3.\n");
                    }
                }
            } while (!privilegioValido);

            // Move o ponteiro do arquivo para o início do registro a ser alterado
            fseek(file, -((long)(sizeof(char) * 12 + sizeof(char) * 10 + sizeof(char) * 50 + sizeof(int))), SEEK_CUR);

            // Escreve os novos dados do usuário no arquivo
            EscreverUsuario(file, &usuario);

            printf("Usuário alterado com sucesso!\n");

            // Verifica se o usuário atual está alterando seu próprio privilégio
            if (strcmp(cpfAtual, cpf) == 0)
            {
                userLoggedOut = 1; // Marca que o usuário atual deve ser desconectado
            }

            break;
        }
    }

    // Se o usuário não foi encontrado, exibe uma mensagem
    if (!encontrado)
    {
        printf("Usuário com CPF '%s' não encontrado.\n", cpf);
    }

    // Fecha o arquivo
    fclose(file);
}

// Função para cadastrar uma nova matéria
void CadastrarMateria()
{
    // Declaração de variáveis para armazenar informações da matéria
    char nomeMateria[100];
    char nomeProf[50];
    int numAlunos;
    Estudante alunos[40];

    while (1)
    {
        // Solicita e lê o nome da matéria
        printf("Digite o nome da matéria: ");
        scanf("%99s", nomeMateria);

        // Verifica se a matéria já existe
        if (!MateriaExiste(nomeMateria))
        {
            break;
        }
        else
        {
            // Informa o usuário que a matéria já existe e retorna da função
            printf("Erro: Matéria já existe.\n");
            system("pause");
            // Limpa o buffer de entrada para remover a entrada inválida
            while (getchar() != '\n');
            system("cls");
        }
    }
    
    // Remove o caractere de nova linha do final da string
    nomeMateria[strcspn(nomeMateria, "\n")] = '\0';

    while (1)
    {
        ListarProfessores();
        // Solicita e lê o nome do professor
        printf("\nDigite o nome do professor: ");
        scanf("%49s", nomeProf);

        // Verifica se o professor existe e tem privilégio de professor
        if (ProfessorExiste(nomeProf))
        {
            break; // Sai do loop se a entrada for válida
        }
        else
        {
            // Informa o usuário que o professor não está cadastrado ou não tem privilégio e retorna da função
            printf("Erro: Professor não cadastrado ou não possui privilégio de professor.\n");
            system("pause");
            // Limpa o buffer de entrada para remover a entrada inválida
            while (getchar() != '\n');
            system("cls");
        }
    }
    
    

    // Solicita o número de alunos até que um valor válido (entre 10 e 40) seja inserido
    while (1)
    {
        // Solicita e lê o número de alunos
        printf("Digite o número de alunos (entre 10 e 40): ");
        if (scanf("%d", &numAlunos) == 1 && numAlunos >= 10 && numAlunos <= 40)
        {
            break; // Sai do loop se a entrada for válida
        }
        else
        {
            // Informa o usuário que o número inserido é inválido
            printf("Número inválido. Por favor, insira um valor entre 10 e 40.\n");
            system("pause");
            // Limpa o buffer de entrada para remover a entrada inválida
            while (getchar() != '\n');
            system("cls");
        }
    }

    // Coleta informações dos alunos
    int i;
	for ( i = 0; i < numAlunos; i++)
    {
        // Define o tamanho máximo do nome do aluno
        alunos[i].tamNome = 50;
        // Aloca memória para armazenar o nome do aluno
        alunos[i].nome = (char *)malloc(alunos[i].tamNome);
        // Solicita e lê o nome do aluno
        printf("Digite o nome do aluno %d: ", i + 1);
        scanf("%49s", alunos[i].nome);
        // Inicializa os dados do aluno
        alunos[i].p1 = 0;
        alunos[i].p2 = 0;
        alunos[i].media = 0;
        alunos[i].faltas = 0;
        // Define a situação inicial do aluno como "Indef"
        strcpy(alunos[i].situ, "Indef");
    }

    // Cria o nome do arquivo para salvar os dados da matéria
    char nomeArquivo[105];
    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.bin", nomeMateria);

    // Abre o arquivo para escrita em modo binário
    FILE *file = fopen(nomeArquivo, "wb");
    if (file == NULL)
    {
        // Informa o usuário sobre o erro ao criar o arquivo e retorna da função
        printf("Erro ao criar o arquivo: %s\n", nomeArquivo);
        return;
    }

    // Escreve os dados da matéria no arquivo
    fwrite(nomeMateria, sizeof(char), 100, file);
    fwrite(nomeProf, sizeof(char), 50, file);
    fwrite(&numAlunos, sizeof(int), 1, file);

    // Escreve os dados dos alunos no arquivo
	for (i = 0; i < numAlunos; i++)
    {
        EscreverEstudante(file, &alunos[i]);
        // Libera a memória alocada para o nome do aluno
        free(alunos[i].nome);
    }

    // Fecha o arquivo
    fclose(file);
    // Informa o usuário que a matéria foi cadastrada com sucesso
    printf("Matéria %s atribuída ao professor %s com %d alunos.\n", nomeMateria, nomeProf, numAlunos);
}

// Função para registrar avisos
void RegistrarAviso()
{
    // Abre o arquivo "avisos.txt" em modo de escrita
    FILE *file = fopen("avisos.txt", "w");
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Declaração de um array para armazenar até 100 avisos, cada um com até 256 caracteres
    char aviso[100][256];
    int num_linhas = 0;
    // Solicita ao usuário que digite os avisos
    printf("Digite o aviso. Para terminar, digite uma linha em branco e pressione Enter:\n");

    // Loop para ler os avisos do usuário
    while (num_linhas < 100)
    {
        fgets(aviso[num_linhas], 256, stdin);

        // Se uma linha em branco for encontrada, parar de ler
        if (strcmp(aviso[num_linhas], "\n") == 0)
        {
            break;
        }

        num_linhas++;
    }

    // Escreve os avisos no arquivo
    int i;
	for ( i = 0; i < num_linhas; i++)
    {
        fprintf(file, "%s", aviso[i]);
    }
    // Fecha o arquivo
    fclose(file);
    // Informa ao usuário que o aviso foi registrado com sucesso
    printf("Aviso registrado com sucesso!\n");
}

// Função para mostrar todos os avisos registrados no arquivo avisos.txt
void MostrarAvisos()
{
    // Abre o arquivo "avisos.txt" em modo de leitura
    FILE *file = fopen("avisos.txt", "r");
    if (file == NULL)
    {
        // Exibe uma mensagem de erro se o arquivo não puder ser aberto
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Declaração de um array para armazenar cada linha do arquivo
    char linha[256];
    // Imprime o cabeçalho dos avisos
    printf("---------------------- Aviso ---------------------\n");

    // Ler todas as linhas do arquivo e imprimir
    while (fgets(linha, sizeof(linha), file))
    {
        // Imprime cada linha do arquivo
        printf("%s", linha);
    }

    // Fecha o arquivo
    fclose(file);
    // Imprime o rodapé dos avisos
    printf("--------------------------------------------------\n");
}

// Menu do Administrador
void Administrador(membroAcademico admin)
{
    char opcao[100];   // Opção do menu
    char escolha[100]; // Variável para armazenar a escolha de sair

    printf("\n-------------- Bem vindo(a), Admin %s! --------------\n", admin.nome);

    // Laço do menu
    do
    {
        // Exibe o menu
        printf("\n\n------------ Insira a opção desejada: ------------\n");
        printf("------------- 1 - Cadastrar usuário -------------\n");
        printf("------------- 2 - Alterar usuário ---------------\n");
        printf("------------- 3 - Cadastrar matéria -------------\n");
        printf("------------- 4 - Registrar aviso ----------------\n");
        printf("------------- 5 - Listar usuários ---------------\n");
        printf("------------- 6 - Logout -------------------------\n");
        scanf("%s", opcao);
        fflush(stdin);
        system("cls");

        // Verifica o comprimento da opção digitada
        if (strlen(opcao) > 1)
        {
            opcao[0] = '7'; // Se o comprimento for maior que 1 define '7' para retornar uma opção inválida
        }

        // Trata a opção escolhida
        switch (opcao[0])
        {
        case '1':
            CadastrarUsuario("login.bin");
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '2':
            AlterarUsuario("login.bin", admin.cpf);
            // Verifica se o usuário deve ser desconectado
            if (userLoggedOut)
            {
                // Código para finalizar a sessão do usuário ou encerrar o programa
                printf("Você alterou seu próprio privilégio. Será necessário fazer logout.\n");
                fflush(stdin);  // Limpa o buffer de entrada do teclado
                getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                system("cls");
                return;
            }
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '3':
            CadastrarMateria();
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '4':
            RegistrarAviso("avisos.txt");
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '5':
            ListarUsuarios("login.bin");
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '6':
            do
            {
                // Pergunta se deseja realmente sair
                printf("Deseja realmente sair?\n1- Sim\n2- Nao\n");
                scanf("%s", escolha);
                fflush(stdin);
                // Verifica o comprimento da escolha de saída
                if (strlen(escolha) > 1)
                {
                    escolha[0] = '3'; // Se o comprimento for maior que 1, define '3' para retornar uma opção inválida
                }
                if (escolha[0] == '1')
                {
                    printf("\nSaindo. . .");
                    fflush(stdin);  // Limpa o buffer de entrada do teclado
                    getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                    system("cls");
                    return; // Retorna ao início do main
                }
                else if (escolha[0] == '2')
                {
                    opcao[0] = '7';
                }
                else
                {
                    printf("Opcao invalida\n");
                    printf("Pressione enter para continuar. . .\n");
                    fflush(stdin); // Limpa o buffer de entrada do teclado
                    getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                }
            } while (escolha[0] != '1' && escolha[0] != '2');
            break;
        default:
            printf("Opcao Invalida\n");
            printf("Pressione enter para continuar. . .\n");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        }

        if (opcao[0] != '6')
        {
            printf("Voltando ao menu...\n");
        }
        system("cls"); // Limpa a tela (no Windows)
    } while (opcao[0] != '6');
}

// Menu do Professor
void Professor(membroAcademico professor)
{
    char opcao[100];   // Opção do menu
    char escolha[100]; // Variável para armazenar a escolha de sair
    char nomeMateria[100];
    char nomeArquivo[105];

    printf("\n\n-------------- Bem vindo(a), Prof. %s! --------------\n\n", professor.nome);

    printf("Digite o nome da matéria que você leciona: ");
    getchar(); // Limpar o buffer de entrada
    fgets(nomeMateria, 100, stdin);
    nomeMateria[strcspn(nomeMateria, "\n")] = '\0';

    if (!ProfessorExisteNaMateria(nomeMateria, professor.nome))
    {
        printf("Erro: Você não está registrado como professor da matéria %s.\n", nomeMateria);
        return;
    }

    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.bin", nomeMateria);

    // Laço do menu
    do
    {
        // Exibe o menu
        printf("\n\n------------ Insira a opção desejada: ------------\n");
        printf("-------------- 1 - Atribuir notas ----------------\n");
        printf("-------------- 2 - Realizar chamada --------------\n");
        printf("-------------- 3 - Fechamento da matéria --------\n");
        printf("-------------- 4 - Logout ------------------------\n");
        scanf("%s", opcao);
        fflush(stdin);
        system("cls");

        // Verifica o comprimento da opção digitada
        if (strlen(opcao) > 1)
        {
            opcao[0] = '5'; // Se o comprimento for maior que 1 define '5' para retornar uma opção inválida
        }

        // Trata a opção escolhida
        switch (opcao[0])
        {
        case '1':
            AtribuirNotas(nomeArquivo);
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '2':
            RealizarChamada(nomeArquivo);
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '3':
            FechamentoMateria(nomeArquivo);
            printf("\nPressione enter para continuar. . .");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        case '4':
            do
            {
                // Pergunta se deseja realmente sair
                printf("Deseja realmente sair?\n1- Sim\n2- Nao\n");
                scanf("%s", escolha);
                fflush(stdin);
                // Verifica o comprimento da escolha de saída
                if (strlen(escolha) > 1)
                {
                    escolha[0] = '3'; // Se o comprimento for maior que 1, define '3' para retornar uma opção inválida
                }
                if (escolha[0] == '1')
                {
                    printf("\nSaindo. . .");
                    fflush(stdin);  // Limpa o buffer de entrada do teclado
                    getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                    system("cls");
                    return; // Retorna ao início do main
                }
                else if (escolha[0] == '2')
                {
                    opcao[0] = '5';
                }
                else
                {
                    printf("Opcao invalida\n");
                    printf("Pressione enter para continuar. . .\n");
                    fflush(stdin); // Limpa o buffer de entrada do teclado
                    getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                }
            } while (escolha[0] != '1' && escolha[0] != '2');
            break;
        default:
            printf("Opcao Invalida\n");
            printf("Pressione enter para continuar. . .\n");
            fflush(stdin); // Limpa o buffer de entrada do teclado
            getchar();     // Aguarda uma tecla ser pressionada antes de continuar
            break;
        }

        if (opcao[0] != '4')
        {
            printf("Voltando ao menu...\n");
        }
        system("cls"); // Limpa a tela (no Windows)
    } while (opcao[0] != '4');
}

// Menu do Aluno
void Aluno(membroAcademico aluno)
{
    char opcao[100];   // Opção do menu
    char escolha[100]; // Variável para armazenar a escolha de sair
    char nomeMateria[100];
    char nomeArquivo[105];

    printf("\n\n-------------- Bem vindo(a), %s! --------------\n\n", aluno.nome);

    MostrarAvisos();

    printf("\n\nDigite o nome da matéria: ");
    getchar(); // Limpar o buffer de entrada
    fgets(nomeMateria, 100, stdin);
    nomeMateria[strcspn(nomeMateria, "\n")] = '\0';

    if (!AlunoExisteNaMateria(nomeMateria, aluno.nome))
    {
        printf("Erro: Você não está registrado na matéria %s.\n", nomeMateria);
        return;
    }

    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.bin", nomeMateria);

    // Laço do menu
    do
    {
        // Exibe o menu
        printf("\n\n------------ Insira a opção desejada: ------------\n");
        printf("------------------- 1 - Notas --------------------\n");
        printf("------------------- 2 - Faltas -------------------\n");
        printf("------------------- 3 - Logout -------------------\n");
        scanf("%s", opcao); 
        fflush(stdin); // Limpa o buffer de entrada do teclado 
        system("cls");      

        // Verifica o comprimento da opção digitada
        if (strlen(opcao) > 1)
        {
            opcao[0] = '5'; // Se o comprimento for maior que 1 define '5' para retornar uma opção inválida
        }

        // Trata a opção escolhida
        switch (opcao[0])
        {
            case '1':
                MostrarNotas(nomeArquivo, aluno.nome); // Mostra as notas do aluno
                printf("\nPressione enter para continuar. . .");
                fflush(stdin); // Limpa o buffer de entrada do teclado
                getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                break;
            case '2':
                MostrarFaltas(nomeArquivo, aluno.nome); // Mostra as faltas do aluno
                printf("\nPressione enter para continuar. . .");
                fflush(stdin); // Limpa o buffer de entrada do teclado
                getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                break;
            case '3':
                do
                {
                    // Pergunta se deseja realmente sair
                    printf("Deseja realmente sair?\n1- Sim\n2- Nao\n");
                    scanf("%s", escolha);
                    fflush(stdin);
                    // Verifica o comprimento da escolha de saída
                    if (strlen(escolha) > 1)
                    {
                        escolha[0] = '3'; // Se o comprimento for maior que 1, define '3' para retornar uma opção inválida
                    }
                    if (escolha[0] == '1')
                    {
                        printf("\nSaindo. . .");
                        fflush(stdin);  // Limpa o buffer de entrada do teclado
                        getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                        system("cls");
                        return; // Retorna ao início do main
                    }
                    else if (escolha[0] == '2')
                    {
                        opcao[0] = '5';
                    }
                    else
                    {
                        printf("Opcao invalida\n");
                        printf("Pressione enter para continuar. . .\n");
                        fflush(stdin); // Limpa o buffer de entrada do teclado
                        getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                        system("cls");
                    }
                } while (escolha[0] != '1' && escolha[0] != '2');
                break;
            default:
                printf("Opcao Invalida\n");
                printf("Pressione enter para continuar. . .\n");
                fflush(stdin); // Limpa o buffer de entrada do teclado
                getchar();     // Aguarda uma tecla ser pressionada antes de continuar
                break;
            }

            if (opcao[0] != '3')
            {
                printf("Voltando ao menu...\n");
            }
            system("cls"); // Limpa a tela (no Windows)
    } while (opcao[0] != '3');
}

// Função principal do sistema
int main()
{
    // Define a codificação de saída do console para UTF-8
    SetConsoleOutputCP(CP_UTF8);

    // Define as configurações de localidade para português do Brasil com UTF-8
    setlocale(LC_ALL, "pt_BR.UTF-8");

    // Nome do arquivo binário que armazena as informações de login
    const char *nomeArquivo = "login.bin";

    // Descomente essa linha se quiser criar o arquivo binário de Login do zero (comente o resto da main até o system pause)
    // FormatarArquivoLogin(nomeArquivo);

    // Variáveis para armazenar o CPF, senha e privilégio do usuário
    char cpf[12];  // Armazena o CPF digitado pelo usuário
    char senha[10]; // Armazena a senha digitada pelo usuário
    membroAcademico usuarioEncontrado; // Estrutura para armazenar os dados do usuário encontrado

    // Loop infinito para o menu do sistema
    while (1)
    {
        // Exibe o cabeçalho do sistema
        printf("\n**************************************\n");
        printf("          Bem vindo ao SIGA!            ");
        printf("\n**************************************\n");

        // Solicita o CPF do usuário
        printf("Digite o CPF: ");
        scanf("%11s", cpf); // Lê o CPF digitado pelo usuário

        // Valida o CPF
        if (!ValidarCPF(cpf)) {
            // Se o CPF for inválido, exibe uma mensagem de erro
            printf("CPF inválido! Certifique-se de que ele contenha apenas números e 11 dígitos.\n");
            system("Pause"); // Pausa o sistema para que o usuário veja a mensagem
            system("cls"); // Limpa a tela
            continue; // Volta ao início do loop
        }

        // Solicita a senha do usuário
        printf("Digite a senha: ");
        scanf("%9s", senha); // Lê a senha digitada pelo usuário

        // Verifica se o CPF e a senha correspondem a um usuário no arquivo de login
        if (Login(nomeArquivo, cpf, senha, &usuarioEncontrado))
        {
            // Limpa a tela após o login bem-sucedido
            system("cls");
            
            // Verifica o privilégio do usuário e chama a função correspondente
            switch (usuarioEncontrado.privilegio)
            {
            case 1:
                Administrador(usuarioEncontrado); // Chama a função do administrador
                break;
            case 2:
                Professor(usuarioEncontrado); // Chama a função do professor
                break;
            case 3:
                Aluno(usuarioEncontrado); // Chama a função do aluno
                break;
            default:
                // Se o privilégio for desconhecido, exibe uma mensagem de erro
                printf("Privilégio desconhecido.\n");
                break;
            }
        }
        else
        {
            // Se o login falhar, exibe uma mensagem de erro
            printf("CPF e/ou senha incorretos.\n");
            system("Pause"); // Pausa o sistema para que o usuário veja a mensagem
            system("cls"); // Limpa a tela
        }

    }

    // Retorna 0 para indicar que o programa terminou corretamente
    return 0;
}


