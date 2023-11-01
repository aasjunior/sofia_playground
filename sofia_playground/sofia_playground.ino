#include <string.h>

enum Etapa{
	MOSTRAR_COMBINACAO,
    AGUARDAR_CLIQUE
};

struct PinsRGB{
	int RED;
  	int BLUE;
  	int GREEN;
  
  	// Construtor
  	PinsRGB(int r_pin, int b_pin, int g_pin) : RED(r_pin), BLUE(b_pin), GREEN(g_pin){
    	pinMode(RED, OUTPUT);
        pinMode(BLUE, OUTPUT);
        pinMode(GREEN, OUTPUT);
    }
};

struct Button{
  	const int PIN;
  	bool STATE;
	unsigned long lastDebounceTime;
  	unsigned long currentMillis;
    const int debounceDelay = 50;
  
  	Button(int pin) : PIN(pin), STATE(false), lastDebounceTime(0), currentMillis(0) {
        pinMode(PIN, INPUT);
    }
};

struct Combinacao{
  String forma;
  String cor;
};

const Combinacao combinacoes[] = {
      {"circulo", "vermelho"},
      {"circulo", "azul"},
      {"circulo", "verde"},
      {"triangulo", "vermelho"},
      {"triangulo", "azul"},
      {"triangulo", "verde"},
      {"quadrado", "vermelho"},
      {"quadrado", "azul"},
      {"quadrado", "verde"}
    };

Button BTN_START(19);

class Teste{
	String forma;
  	String cor;
  
  	public:
  	// Construtor padrão
  	Teste() : forma(""), cor("") {}
  
  	// Construtor com argumentos 
    Teste(const String& f, const String& c) : forma(f), cor(c) {}
  
  	String getForma() const{return forma;}
  
  	String getCor() const{return cor;}
  
};

class Forma{
	const String nome;
  	const PinsRGB LED_PIN;
  	Button BTN;
  
  	public:
  	Forma(const String& n, const PinsRGB& l_pin, int b_pin) 
      : nome(n), LED_PIN(l_pin), BTN(b_pin){}
  
  	String getNome() const{return nome;}
  
  	void acenderLED(const int corRGB[]){
      digitalWrite(LED_PIN.RED, corRGB[0]);
      digitalWrite(LED_PIN.BLUE, corRGB[1]);
      digitalWrite(LED_PIN.GREEN, corRGB[2]);
  	}
  
  	bool getBTN_STATE(){
      int btnState = digitalRead(BTN.PIN);
      
  	  // Tratamento para o efeito bouncing no clique do botão
      if(btnState != BTN.STATE){
      	BTN.lastDebounceTime = BTN.currentMillis;	
      }
      if(millis() - BTN.lastDebounceTime > BTN.debounceDelay){
        if(btnState == HIGH && !BTN.STATE){
        	BTN.STATE = true;
        }
      }
      if(btnState == LOW){
      	BTN.STATE = false;
      }
      
      return BTN.STATE;
    }
};

// Declaração das formas e dos pinos referentes ao LED RGB e ao botão
Forma circulo("circulo", {13,12,11}, 4);
Forma triangulo("triangulo", {10,9,8}, 3);
Forma quadrado("quadrado", {7,6,5}, 2);

// Prototipos das funções
Teste* gerarTestes(int qtTestes);
void mapearCorRGB(const String& cor);
void imprimirCombinacao(const Teste& teste, int n);
void iniciarTeste(Teste* testes, int qtTestes);

void setup(){
  Serial.begin(9600);
  
  // Embaralha a função random para que não inicialize com os mesmos valores
  randomSeed(analogRead(0));
}

void imprimirCombinacao(const Teste& teste, int n){
	Serial.print("Combinacao ");
    Serial.print(n);
    Serial.print(": ");
    Serial.println(teste.getForma() + " " + teste.getCor());
}

float temporizador(unsigned long& tempoInicio,unsigned long tempoLimite){
  if(tempoInicio == 0){
  	tempoInicio = millis();
  }
  
  // Verifica se algum botão foi pressionado
  if(circulo.getBTN_STATE() || triangulo.getBTN_STATE() || quadrado.getBTN_STATE()){
    unsigned long tempoDecorrido = millis() - tempoInicio;
    tempoInicio = 0;
    return static_cast<float>(tempoDecorrido) / 1000.0;
  }
  if(millis() - tempoInicio > tempoLimite){
  	tempoInicio = 0;
    return -1.0; // Tempo limite atingido sem resposta
  }
  
  return -2.0; // Ainda aguardando o clique
}

void mapearCorRGB(const String& cor){
  int init[] = {0, 0, 0}; // Inicializa com os LEDs apagados
  
  if(cor == "vermelho"){
  	init[0] = 255;
  }else if(cor == "azul"){
  	init[1] = 255;
  }else if(cor == "verde"){
  	init[2] = 255;
  }
  
  circulo.acenderLED(init);
  triangulo.acenderLED(init);
  quadrado.acenderLED(init);
}

void apagarLED(){
  	int low[] = {0,0,0};
  
  	circulo.acenderLED(low);
    triangulo.acenderLED(low);
    quadrado.acenderLED(low);
}
void iniciarTeste(Teste* testes, int qtTestes) {
  int i = 0;
  Etapa etapa = MOSTRAR_COMBINACAO;

  while (i < qtTestes) {
    switch (etapa) {
      case MOSTRAR_COMBINACAO:
        mapearCorRGB(testes[i].getCor());
        imprimirCombinacao(testes[i], i);
        etapa = AGUARDAR_CLIQUE;
        break;

      case AGUARDAR_CLIQUE:
        if (circulo.getBTN_STATE()) {
          
          if (circulo.getNome() == testes[i].getForma()) {
            Serial.println(circulo.getNome());
            apagarLED();
            delay(500);
            i++;
            etapa = MOSTRAR_COMBINACAO;
          }else{
          	Serial.println("Forma incorreta: " + circulo.getNome());
          	delay(500);
          }
        } else if (triangulo.getBTN_STATE()) {
          
          if (triangulo.getNome() == testes[i].getForma()) {
            Serial.println(triangulo.getNome());
            apagarLED();
            delay(500);
            i++;
            etapa = MOSTRAR_COMBINACAO;
          }else{
          	Serial.println("Forma incorreta: " + triangulo.getNome());
            delay(500);
          }
        } else if (quadrado.getBTN_STATE()) {
          
          if (quadrado.getNome() == testes[i].getForma()) {
          	Serial.println(quadrado.getNome());
            apagarLED();
            delay(500);
            i++;
            etapa = MOSTRAR_COMBINACAO;
          }else{
          	Serial.println("Forma incorreta: " + quadrado.getNome());
            delay(500);
          }
        }
        break;
    }
  }
}



Teste* gerarTestes(int qtTestes){
  	// Gera combinações aleatórias entre formas e cores para realização do teste
  	int totalCombinacoes = sizeof(combinacoes) / sizeof(combinacoes[0]);
  
	if (qtTestes > totalCombinacoes) {
      qtTestes = totalCombinacoes;
    }
  
  	int testesGerados = 0;
  	
  	Teste* testes = new Teste[qtTestes];
  
    // Vetor de índices para controlar as combinações já usadas
    int* indicesUsados = new int[totalCombinacoes];
    for (int i = 0; i < totalCombinacoes; i++) {
      indicesUsados[i] = 0;
    }

  	// Embaralha a combinação entre formas e cores
    while (testesGerados < qtTestes) {
      int indiceCombinacao = random(totalCombinacoes);

      if (indicesUsados[indiceCombinacao] == 0) {
        testes[testesGerados] = Teste(combinacoes[indiceCombinacao].forma, combinacoes[indiceCombinacao].cor);
        indicesUsados[indiceCombinacao] = 1;
        testesGerados++;
      }
    }
  
  	// Libere a memória alocada para o vetor de índices usados
  	delete[] indicesUsados;
  	
  	return testes;
}

void loop() {
  BTN_START.STATE = digitalRead(BTN_START.PIN);

  if (BTN_START.STATE) {
    Teste testes[2]; // Ajustando para gerar apenas 2 combinações únicas
    int totalCombinacoes = sizeof(combinacoes) / sizeof(combinacoes[0]);
    int indicePrimeiraCombinacao = random(totalCombinacoes);

    // Escolhe a primeira combinação aleatória
    testes[0] = Teste(combinacoes[indicePrimeiraCombinacao].forma, combinacoes[indicePrimeiraCombinacao].cor);

    // Escolhe a segunda combinação única
    int indiceSegundaCombinacao = (indicePrimeiraCombinacao + 1) % totalCombinacoes;
    testes[1] = Teste(combinacoes[indiceSegundaCombinacao].forma, combinacoes[indiceSegundaCombinacao].cor);

    iniciarTeste(testes, 2);
  }
}
