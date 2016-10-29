#include <math.h>
#include <string.h>		// para usar strings

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Rotinas para leitura de arquivos .hdr
#include "rgbe.h"

// Variáveis globais a serem utilizadas:
RGBf* image_aux;

// Dimensões da imagem de entrada
int sizeX, sizeY;

// Imagem de entrada
RGBf* image;

// Imagem de saída
RGB8* image8;

// Fator de exposição inicial
float exposure = 1.0;

// Modo de exibição atual
int modo;

// Função pow mais eficiente (cerca de 7x mais rápida)
float fastpow(float a, float b);
float fastpow(float a, float b) {
     union { float f; int i; }
      u = { a };
      u.i = (int)(b * (u.i - 1065307417) + 1065307417);
      return u.f;
}

float escala(float cor, float c);
float escala(float cor, float c)
{
    return cor / (cor + c);
}

float correcao_gama(float cor, float gama);
float correcao_gama(float cor, float gama)
{
    return fastpow(cor, (1.0 / gama));
}

unsigned char conversao_para_24_bits(float cor);
unsigned char conversao_para_24_bits(float cor)
{
    float menor = fminf(1.0, cor);
    menor = menor * 255;
    return (unsigned char)menor;
}

// Função principal de processamento: ela deve chamar outras funções
// quando for necessário (ex: algoritmos de tone mapping, etc)
void process()
{
    printf("Exposure: %.3f\n", exposure);
    //
    // EXEMPLO: preenche a imagem com pixels cor de laranja...
    //
    //
    // SUBSTITUA este código pelos algoritmos a serem implementados
    //
    int pos;
    for(pos=0; pos<sizeX*sizeY; pos++) {
        //aplica o fator de exposicao
        image_aux[pos].r = (image[pos].r * exposure);
        image_aux[pos].g = (image[pos].g * exposure);
        image_aux[pos].b = (image[pos].b * exposure);

        if (modo == SCALE) {
            image_aux[pos].r = escala(image_aux[pos].r, 0.5f);
            image_aux[pos].g = escala(image_aux[pos].g, 0.5f);
            image_aux[pos].b = escala(image_aux[pos].b, 0.5f);
        }
        else { // nesse caso, modo é igual a GAMMA
            image_aux[pos].r = correcao_gama(image_aux[pos].r, 2.0f);
            image_aux[pos].g = correcao_gama(image_aux[pos].g, 2.0f);
            image_aux[pos].b = correcao_gama(image_aux[pos].b, 2.0f);
        }

        image8[pos].r = conversao_para_24_bits(image_aux[pos].r);
        image8[pos].g = conversao_para_24_bits(image_aux[pos].g);
        image8[pos].b = conversao_para_24_bits(image_aux[pos].b);
    }

    //
    // NÃO ALTERAR A PARTIR DAQUI!!!!
    //
    buildTex();
}

int main(int argc, char** argv)
{
    if(argc==1) {
        printf("hdrvis [image file.hdr]\n");
        exit(1);
    }

    // Inicialização da janela gráfica
    init(argc,argv);

    //
    // INCLUA aqui o código para LER a imagem de entrada
    //
    // Siga as orientações no enunciado para:
    //
    // 1. Descobrir o tamanho da imagem (ler header)
    // 2. Ler os pixels
    //

    // Abre o arquivo
    FILE* arq = fopen("imagens-hdr/memorial.hdr","rb");

    // Lê o header do arquivo, de onde são extraídas a largura e altura
    RGBE_ReadHeader(arq, &sizeX, &sizeY, NULL);

    // Aloca imagem float
    image = (RGBf *)malloc(sizeof(RGBf) * sizeX * sizeY);
    image_aux = (RGBf *)malloc(sizeof(RGBf) * sizeX * sizeY);

    // Aloca memória para imagem de 24 bits
    image8 = (RGB8*) malloc(sizeof(RGB8) * sizeX * sizeY);

    // Finalmente, lê a imagem para a memória
    int result = RGBE_ReadPixels_RLE(arq, (float*)image, sizeX, sizeY);
    if (result == RGBE_RETURN_FAILURE) {

    }
    fclose(arq);

    exposure = 1.0f; // exposição inicial

    // Aplica processamento inicial
    process();

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}

