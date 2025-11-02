#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// ESTRUTURAS DE DADOS
// ============================================

// Estrutura para árvore binária da mansão
typedef struct NoSala {
    char nome[50];
    char pista[100];  // Pista encontrada nesta sala (se houver)
    struct NoSala *esquerda;
    struct NoSala *direita;
} NoSala;

// Estrutura para árvore de busca (BST) de pistas
typedef struct NoPista {
    char pista[100];
    struct NoPista *esquerda;
    struct NoPista *direita;
} NoPista;

// Estrutura para tabela hash (pista -> suspeito)
typedef struct NoHash {
    char pista[100];
    char suspeito[50];
    struct NoHash *proximo;  // Para encadeamento em caso de colisão
} NoHash;

#define TAM_HASH 20

// ============================================
// FUNÇÕES DA ÁRVORE BINÁRIA (MANSÃO)
// ============================================

NoSala* criarSala(const char* nome, const char* pista) {
    NoSala* nova = (NoSala*)malloc(sizeof(NoSala));
    strcpy(nova->nome, nome);
    if (pista != NULL) {
        strcpy(nova->pista, pista);
    } else {
        nova->pista[0] = '\0';
    }
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

void explorarSalas(NoSala* raiz, NoPista** pistasEncontradas, NoHash* tabelaHash[]) {
    if (raiz == NULL) {
        printf("\n[!] Você chegou a um beco sem saída!\n");
        return;
    }
    
    printf("\n===========================================\n");
    printf("📍 Você está em: %s\n", raiz->nome);
    printf("===========================================\n");
    
    // Verifica se há pista nesta sala
    if (strlen(raiz->pista) > 0) {
        printf("🔍 Você encontrou uma pista: \"%s\"\n", raiz->pista);
        // Adiciona a pista na BST (implementado mais adiante)
    }
    
    // Se é folha, fim do caminho
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        printf("\n[!] Este é o fim do corredor. Voltando...\n");
        return;
    }
    
    // Menu de navegação
    printf("\n🚪 Para onde deseja ir?\n");
    if (raiz->esquerda != NULL) {
        printf("  [e] Esquerda → %s\n", raiz->esquerda->nome);
    }
    if (raiz->direita != NULL) {
        printf("  [d] Direita → %s\n", raiz->direita->nome);
    }
    printf("  [s] Sair da exploração\n");
    printf("\nEscolha: ");
    
    char escolha;
    scanf(" %c", &escolha);
    
    switch(escolha) {
        case 'e':
        case 'E':
            if (raiz->esquerda != NULL) {
                explorarSalas(raiz->esquerda, pistasEncontradas, tabelaHash);
            } else {
                printf("\n[!] Não há caminho à esquerda!\n");
            }
            break;
        case 'd':
        case 'D':
            if (raiz->direita != NULL) {
                explorarSalas(raiz->direita, pistasEncontradas, tabelaHash);
            } else {
                printf("\n[!] Não há caminho à direita!\n");
            }
            break;
        case 's':
        case 'S':
            printf("\n[✓] Saindo da exploração...\n");
            return;
        default:
            printf("\n[!] Opção inválida!\n");
            explorarSalas(raiz, pistasEncontradas, tabelaHash);
    }
}

// ============================================
// FUNÇÕES DA ÁRVORE DE BUSCA (BST - PISTAS)
// ============================================

NoPista* inserirPista(NoPista* raiz, const char* pista) {
    if (raiz == NULL) {
        NoPista* novo = (NoPista*)malloc(sizeof(NoPista));
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }
    
    int comparacao = strcmp(pista, raiz->pista);
    
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // Se for igual, não insere duplicata
    
    return raiz;
}

void exibirPistasEmOrdem(NoPista* raiz) {
    if (raiz != NULL) {
        exibirPistasEmOrdem(raiz->esquerda);
        printf("  📋 %s\n", raiz->pista);
        exibirPistasEmOrdem(raiz->direita);
    }
}

NoPista* buscarPista(NoPista* raiz, const char* pista) {
    if (raiz == NULL || strcmp(raiz->pista, pista) == 0) {
        return raiz;
    }
    
    if (strcmp(pista, raiz->pista) < 0) {
        return buscarPista(raiz->esquerda, pista);
    } else {
        return buscarPista(raiz->direita, pista);
    }
}

// ============================================
// FUNÇÕES DA TABELA HASH (PISTAS → SUSPEITOS)
// ============================================

int funcaoHash(const char* chave) {
    int hash = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        hash += (int)chave[i];
    }
    return hash % TAM_HASH;
}

void inserirNaHash(NoHash* tabela[], const char* pista, const char* suspeito) {
    int indice = funcaoHash(pista);
    
    // Cria novo nó
    NoHash* novo = (NoHash*)malloc(sizeof(NoHash));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->proximo = NULL;
    
    // Inserção com encadeamento
    if (tabela[indice] == NULL) {
        tabela[indice] = novo;
    } else {
        // Adiciona no início da lista encadeada
        novo->proximo = tabela[indice];
        tabela[indice] = novo;
    }
    
    printf("  ✓ Pista \"%s\" associada ao suspeito: %s\n", pista, suspeito);
}

void consultarPistaHash(NoHash* tabela[], const char* pista) {
    int indice = funcaoHash(pista);
    NoHash* atual = tabela[indice];
    
    printf("\n🔍 Buscando suspeitos relacionados à pista: \"%s\"\n", pista);
    
    int encontrou = 0;
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            printf("  👤 Suspeito: %s\n", atual->suspeito);
            encontrou = 1;
        }
        atual = atual->proximo;
    }
    
    if (!encontrou) {
        printf("  [!] Nenhum suspeito associado a esta pista.\n");
    }
}

void exibirTodasAssociacoes(NoHash* tabela[]) {
    printf("\n===========================================\n");
    printf("📊 TODAS AS ASSOCIAÇÕES PISTA → SUSPEITO\n");
    printf("===========================================\n");
    
    for (int i = 0; i < TAM_HASH; i++) {
        NoHash* atual = tabela[i];
        while (atual != NULL) {
            printf("  🔗 \"%s\" → %s\n", atual->pista, atual->suspeito);
            atual = atual->proximo;
        }
    }
}

void encontrarSuspeitoMaisCitado(NoHash* tabela[]) {
    // Array para contar citações de cada suspeito
    char suspeitos[100][50];
    int contagens[100];
    int totalSuspeitos = 0;
    
    // Percorre a tabela hash
    for (int i = 0; i < TAM_HASH; i++) {
        NoHash* atual = tabela[i];
        while (atual != NULL) {
            // Procura se o suspeito já foi contado
            int encontrado = 0;
            for (int j = 0; j < totalSuspeitos; j++) {
                if (strcmp(suspeitos[j], atual->suspeito) == 0) {
                    contagens[j]++;
                    encontrado = 1;
                    break;
                }
            }
            
            // Se não foi encontrado, adiciona novo suspeito
            if (!encontrado && totalSuspeitos < 100) {
                strcpy(suspeitos[totalSuspeitos], atual->suspeito);
                contagens[totalSuspeitos] = 1;
                totalSuspeitos++;
            }
            
            atual = atual->proximo;
        }
    }
    
    // Encontra o mais citado
    if (totalSuspeitos == 0) {
        printf("\n[!] Nenhum suspeito foi registrado ainda.\n");
        return;
    }
    
    int maxContagem = 0;
    int indiceMaisCitado = 0;
    
    for (int i = 0; i < totalSuspeitos; i++) {
        if (contagens[i] > maxContagem) {
            maxContagem = contagens[i];
            indiceMaisCitado = i;
        }
    }
    
    printf("\n===========================================\n");
    printf("🕵️ ANÁLISE DE SUSPEITOS\n");
    printf("===========================================\n");
    
    for (int i = 0; i < totalSuspeitos; i++) {
        printf("  👤 %s: %d pista(s)\n", suspeitos[i], contagens[i]);
    }
    
    printf("\n🎯 PRINCIPAL SUSPEITO: %s (%d pistas encontradas)\n", 
           suspeitos[indiceMaisCitado], maxContagem);
}

// ============================================
// FUNÇÃO PARA CONSTRUIR A MANSÃO
// ============================================

NoSala* construirMansao() {
    // Criando a estrutura da mansão
    NoSala* raiz = criarSala("Hall de Entrada", NULL);
    
    // Nível 1
    raiz->esquerda = criarSala("Biblioteca", "Livro com páginas rasgadas");
    raiz->direita = criarSala("Sala de Estar", "Taça de vinho quebrada");
    
    // Nível 2 - Esquerda
    raiz->esquerda->esquerda = criarSala("Escritório", "Carta anônima");
    raiz->esquerda->direita = criarSala("Sala de Leitura", "Marca de sangue no tapete");
    
    // Nível 2 - Direita
    raiz->direita->esquerda = criarSala("Cozinha", "Faca com resíduos");
    raiz->direita->direita = criarSala("Jardim de Inverno", "Pegadas na lama");
    
    // Nível 3 - Folhas
    raiz->esquerda->esquerda->esquerda = criarSala("Cofre Secreto", "Documento comprometedor");
    raiz->esquerda->direita->direita = criarSala("Varanda dos Fundos", "Cigarro apagado às pressas");
    raiz->direita->esquerda->esquerda = criarSala("Despensa", "Veneno para ratos");
    raiz->direita->direita->direita = criarSala("Estufa", "Luvas sujas de terra");
    
    return raiz;
}

// ============================================
// MENU PRINCIPAL
// ============================================

void exibirMenu() {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║     🔍 DETECTIVE QUEST - MESTRE 🔍   ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("\n[1] 🏛️  Explorar a Mansão\n");
    printf("[2] 📋 Ver Pistas Coletadas (em ordem)\n");
    printf("[3] 🔗 Registrar Pista → Suspeito\n");
    printf("[4] 🔍 Consultar Suspeito de uma Pista\n");
    printf("[5] 📊 Ver Todas as Associações\n");
    printf("[6] 🎯 Revelar Principal Suspeito\n");
    printf("[0] 🚪 Sair do Jogo\n");
    printf("\nEscolha uma opção: ");
}

// ============================================
// FUNÇÃO PRINCIPAL
// ============================================

int main() {
    NoSala* mansao = construirMansao();
    NoPista* pistasColetadas = NULL;
    NoHash* tabelaHash[TAM_HASH];
    
    // Inicializa tabela hash
    for (int i = 0; i < TAM_HASH; i++) {
        tabelaHash[i] = NULL;
    }
    
    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║  BEM-VINDO AO DETECTIVE QUEST - NÍVEL MESTRE!        ║\n");
    printf("║                                                       ║\n");
    printf("║  Um crime ocorreu na mansão. Sua missão é explorar   ║\n");
    printf("║  os cômodos, coletar pistas e identificar o culpado!  ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    // Pré-cadastrar algumas associações para demonstração
    printf("\n[Sistema] Carregando banco de dados de suspeitos...\n\n");
    inserirNaHash(tabelaHash, "Livro com páginas rasgadas", "Professora Helena");
    inserirNaHash(tabelaHash, "Taça de vinho quebrada", "Mordomo James");
    inserirNaHash(tabelaHash, "Carta anônima", "Sr. Ricardo");
    inserirNaHash(tabelaHash, "Marca de sangue no tapete", "Mordomo James");
    inserirNaHash(tabelaHash, "Faca com resíduos", "Chef Antoine");
    inserirNaHash(tabelaHash, "Pegadas na lama", "Jardineiro Paulo");
    inserirNaHash(tabelaHash, "Documento comprometedor", "Sr. Ricardo");
    inserirNaHash(tabelaHash, "Cigarro apagado às pressas", "Mordomo James");
    inserirNaHash(tabelaHash, "Veneno para ratos", "Chef Antoine");
    inserirNaHash(tabelaHash, "Luvas sujas de terra", "Jardineiro Paulo");
    
    // Adicionar pistas na BST
    pistasColetadas = inserirPista(pistasColetadas, "Livro com páginas rasgadas");
    pistasColetadas = inserirPista(pistasColetadas, "Taça de vinho quebrada");
    pistasColetadas = inserirPista(pistasColetadas, "Carta anônima");
    pistasColetadas = inserirPista(pistasColetadas, "Marca de sangue no tapete");
    pistasColetadas = inserirPista(pistasColetadas, "Faca com resíduos");
    pistasColetadas = inserirPista(pistasColetadas, "Pegadas na lama");
    pistasColetadas = inserirPista(pistasColetadas, "Documento comprometedor");
    pistasColetadas = inserirPista(pistasColetadas, "Cigarro apagado às pressas");
    pistasColetadas = inserirPista(pistasColetadas, "Veneno para ratos");
    pistasColetadas = inserirPista(pistasColetadas, "Luvas sujas de terra");
    
    int opcao;
    do {
        exibirMenu();
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 1:
                printf("\n🚪 Entrando na mansão...\n");
                explorarSalas(mansao, &pistasColetadas, tabelaHash);
                break;
                
            case 2:
                printf("\n===========================================\n");
                printf("📋 PISTAS COLETADAS (Ordem Alfabética)\n");
                printf("===========================================\n");
                if (pistasColetadas == NULL) {
                    printf("  [!] Nenhuma pista coletada ainda.\n");
                } else {
                    exibirPistasEmOrdem(pistasColetadas);
                }
                break;
                
            case 3: {
                printf("\n===========================================\n");
                printf("🔗 REGISTRAR NOVA ASSOCIAÇÃO\n");
                printf("===========================================\n");
                
                char pista[100], suspeito[50];
                printf("Digite a pista: ");
                getchar(); // Limpa buffer
                fgets(pista, 100, stdin);
                pista[strcspn(pista, "\n")] = 0; // Remove \n
                
                printf("Digite o suspeito: ");
                fgets(suspeito, 50, stdin);
                suspeito[strcspn(suspeito, "\n")] = 0;
                
                inserirNaHash(tabelaHash, pista, suspeito);
                
                // Adiciona pista na BST se não existir
                if (buscarPista(pistasColetadas, pista) == NULL) {
                    pistasColetadas = inserirPista(pistasColetadas, pista);
                    printf("  ✓ Pista adicionada ao catálogo.\n");
                }
                break;
            }
                
            case 4: {
                char pista[100];
                printf("\nDigite a pista para consultar: ");
                getchar();
                fgets(pista, 100, stdin);
                pista[strcspn(pista, "\n")] = 0;
                
                consultarPistaHash(tabelaHash, pista);
                break;
            }
                
            case 5:
                exibirTodasAssociacoes(tabelaHash);
                break;
                
            case 6:
                encontrarSuspeitoMaisCitado(tabelaHash);
                break;
                
            case 0:
                printf("\n👋 Obrigado por jogar Detective Quest!\n");
                printf("   Até a próxima investigação, detetive! 🕵️\n\n");
                break;
                
            default:
                printf("\n[!] Opção inválida! Tente novamente.\n");
        }
        
    } while(opcao != 0);
    
    // Liberação de memória seria implementada aqui em produção
    
    return 0;
}