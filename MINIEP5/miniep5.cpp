#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <cstdint>  // Para usar intptr_t

using namespace std;

#define SAPO 'S'
#define RAN 'R'
#define VAZIO '_'

// Variáveis globais
vector<char> pedras;
int qtdSapos, qtdRans, posVazia;
int contadorImpasse = 0;
const int maxContadorImpasse = 1000;
pthread_mutex_t trava;

// Função para imprimir o estado atual das pedras
void imprimePedras() {
    for (char pedra : pedras) {
        cout << pedra << " ";
    }
    cout << endl;
}

// Função da thread dos sapos
void* puloSapo(void* arg) {
    intptr_t posicao = (intptr_t)arg;  // Corrigido para intptr_t

    while (true) {
        pthread_mutex_lock(&trava);

        if (posicao + 1 == posVazia) {  // Sapo se move para uma pedra vazia adjacente
            swap(pedras[posicao], pedras[posVazia]);
            posVazia = posicao;
            posicao++;
            contadorImpasse = 0;
        } else if (posicao + 2 == posVazia && pedras[posicao + 1] == RAN) {  // Sapo pula sobre uma rã
            swap(pedras[posicao], pedras[posVazia]);
            posVazia = posicao;
            posicao += 2;
            contadorImpasse = 0;
        } else {
            contadorImpasse++;
        }

        pthread_mutex_unlock(&trava);

        usleep(100000);  // Adiciona um pequeno atraso
        if (contadorImpasse > maxContadorImpasse) break;  // Encerra no impasse
    }

    pthread_exit(nullptr);
}

// Função da thread das rãs
void* puloRan(void* arg) {
    intptr_t posicao = (intptr_t)arg;  // Corrigido para intptr_t

    while (true) {
        pthread_mutex_lock(&trava);

        if (posicao - 1 == posVazia) {  // Rã se move para uma pedra vazia adjacente
            swap(pedras[posicao], pedras[posVazia]);
            posVazia = posicao;
            posicao--;
            contadorImpasse = 0;
        } else if (posicao - 2 == posVazia && pedras[posicao - 1] == SAPO) {  // Ran pula sobre um sapo
            swap(pedras[posicao], pedras[posVazia]);
            posVazia = posicao;
            posicao -= 2;
            contadorImpasse = 0;
        } else {
            contadorImpasse++;
        }

        pthread_mutex_unlock(&trava);

        usleep(100000);  // Adiciona um pequeno atraso
        if (contadorImpasse > maxContadorImpasse) break;  // Encerra no impasse
    }

    pthread_exit(nullptr);
}

// Thread árbitro para detectar impasses
void* arbitro(void* arg) {
    while (true) {
        pthread_mutex_lock(&trava);
        if (contadorImpasse > maxContadorImpasse) {
            cout << "Impasse detectado!" << endl;
            pthread_mutex_unlock(&trava);
            break;
        }
        pthread_mutex_unlock(&trava);

        usleep(500000);  // Verifica a cada 0,5 segundos
    }

    pthread_exit(nullptr);
}

int main() {
    cout << "Digite o número de sapos e rans: ";
    cin >> qtdSapos >> qtdRans;

    int qtdPedras = qtdSapos + qtdRans + 1;
    pedras.resize(qtdPedras);

    // Inicializa as pedras com sapos, rãs e um espaço vazio
    for (int i = 0; i < qtdSapos; i++) pedras[i] = SAPO;
    pedras[qtdSapos] = VAZIO;
    for (int i = qtdSapos + 1; i < qtdPedras; i++) pedras[i] = RAN;

    posVazia = qtdSapos;  // Posição da pedra vazia

    imprimePedras();

    pthread_mutex_init(&trava, nullptr);

    vector<pthread_t> sapos(qtdSapos);
    vector<pthread_t> rans(qtdRans);
    pthread_t threadArbitro;

    // Cria as threads dos sapos
    for (int i = 0; i < qtdSapos; i++) {
        int* arg = new int(i);
        pthread_create(&sapos[i], nullptr, puloSapo, (void*)(intptr_t)i);  // Corrigido para intptr_t
    }

    // Cria as threads das rãs
    for (int i = qtdSapos + 1; i < qtdPedras; i++) {
        int* arg = new int(i);
        pthread_create(&rans[i - (qtdSapos + 1)], nullptr, puloRan, (void*)(intptr_t)i);  // Corrigido para intptr_t
    }

    // Cria a thread do árbitro
    pthread_create(&threadArbitro, nullptr, arbitro, nullptr);

    // Aguarda todas as threads dos sapos e rãs terminarem
    for (int i = 0; i < qtdSapos; i++) pthread_join(sapos[i], nullptr);
    for (int i = 0; i < qtdRans; i++) pthread_join(rans[i], nullptr);

    // Aguarda a thread do árbitro terminar
    pthread_join(threadArbitro, nullptr);

    pthread_mutex_destroy(&trava);

    cout << "Estado final: " << endl;
    imprimePedras();

    return 0;
}
