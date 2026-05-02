typedef struct usuario{
    char nome[15];
    char senha[5];
} usuario;

typedef struct cartao_local{
    char numero_cartao[10];
    char senha_cartao[8];
}cartao_local1;

unsigned char autenticar_usuario(usuario *novo_usuario , unsigned char leitura_teclado[], unsigned char *indice_out );

unsigned char mascara_autentica_senha( usuario *novo_usuario, unsigned char tamanho_senha, unsigned char *indice_out);

/*unsigned char mascara_autentica_senha(usuario *novo_usuario,
                                      unsigned char tamanho_senha,
                                      unsigned char *indice_out,
                                      unsigned char *tecla) ;*/