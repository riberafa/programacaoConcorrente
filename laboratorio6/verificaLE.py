# Define funções para verificar a lógica de execução de uma aplicação leitor/escritor
class LE:
    def __init__(self):
        self.escritores = 0
        self.leitores = 0
        self.leitores_esperando = 0

    def leitorBloqueado(self, id):
        '''Recebe o id do leitor. Verifica se a decisão de bloqueio está correta.'''
        if self.escritores > 0:
            print(f"ERRO: leitor {id} bloqueado quando há escritor escrevendo!")

    def escritorBloqueado(self, id):
        '''Recebe o id do escritor. Verifica se a decisão de bloqueio está correta.'''
        if self.escritores > 0 or self.leitores > 0:
            print(f"ERRO: escritor {id} bloqueado quando há outro escritor ou leitores!")

    def leitorLendo(self, id):
        '''Recebe o id do leitor, verifica se pode ler e registra que está lendo.'''
        if self.escritores > 0 or self.leitores_esperando > 0:
            print(f"ERRO: leitor {id} está lendo quando há escritor(es) esperando para escrever!")
        self.leitores += 1

    def escritorEscrevendo(self, id):
        '''Recebe o id do escritor, verifica se pode escrever e registra que está escrevendo'''
        if self.escritores > 0 or self.leitores > 0:
            self.leitores_esperando += 1
            print(f"Escritor {id} esperando para escrever...")
        else:
            self.escritores += 1
            print(f"Escritor {id} escrevendo...")

    def leitorSaindo(self, id):
        '''Recebe o id do leitor e registra que terminou a leitura.'''
        self.leitores -= 1
        if self.leitores == 0 and self.leitores_esperando > 0:
            print("Liberando escritor(es)...")
            self.escritores += 1
            self.leitores_esperando = 0

    def escritorSaindo(self, id):
        '''Recebe o id do escritor e registra que terminou a leitura.'''
        self.escritores -= 1
