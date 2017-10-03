// АВТОЗАПУСК ГЕНЕРАТОРА на Arduino
// АВР автоматический ввод резерва
// created by ss2s
// https://github.com/ss2s/AutoGeneratorArduino

// ПРИНЦЫП РАБОТЫ:
// Arduino мониторит 2 канала от сети и от генратора (входы сети и генератора подключать через реле перекидное 5в и 0в)
// если 5в то сеть есть, если 0в то сети нет.
// если СЕТЬ пропадает то запускается генератор: зажигание+, подсос+, стартер+, стартер-, подсос-, контактор сети+.
// если СЕТЬ появляеться то генератор отключается: контактор сети-, задержка, зажигание-
// СИЛОВОЙ КОНТАКТОР ПЕРЕКИДНОЙ!

// КАК ПОДКЛЮЧАТЬ:
// Подключите реле как указано в разделе распиновка (можно изменить в разделе настройка:распиновка)
// Вход 5в от сети и генератора подключять через РЕЛЕ . тоесть реле перекидывает контакт между 5в и 0в земля.
// по умолчанию: 5в сеть есть. сеть пропала реле перекинуло на 0в.
// СИЛОВОЙ КОНТАКТОР ПЕРЕКИДНОЙ!

// Кнопка СТОП програмно подтянута к 5в. для активации замкнуть на землю(можно через реле нормально разомкнутое)
// кнопка СТОП предназначена для предотвращения запуска генератора в случае аварии в сети. например если МЧС отключит свет

// провода использовать экранированые и как можно короче (для защиты от наводок)
// микроконтроллер желательно разместить в заземленном экранированом корпусе.

// в разделе настройка:временные задержки - можно настроить временные задержки в миллисекундах. 1000 мс = 1 с

// СВЕТОДИОДНАЯ ИНДИКАЦИЯ:
// мигает раз в секунду - нормальный режим работы. мониторит сеть.
// 3 раза мигает пред запуском генератора и по 2 раза перед каждой последующей попыткой. если с первого раза не запустился.
// 4 раза мигает за 30сек. до выключения зажигания и 2 раза перед выключением.
// 5 миганий 3сек. пауза - ошибка. двигатель не запустился после n попыток.
// 2 мигание 1сек. пауза - ошибка. пришел сигнал СТОП.


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


// ***НАСТРОЙКА*** //

// для настройки изменять цифры
// ВХОДЫ РЕЛЕ 5в СЕТЬ И 5в ГЕНЕРАТОР ПОДКЛЮЧАТЬ ЧРЕЗ ПЕРЕКИДНОЕ РЕЛЕ(5в СЕТЬ ЕСТЬ. 0в СЕТИ НЕТ)
// КНОПКА СТОП ПРОГРАМНО ПОДТЯНУТА К 5в. ДЛЯ АКТИВАЦИИ ЗАМКНУТЬ НА ЗЕМЛЮ


// РАСПИНОВКА
#define IN_CETb_PIN 2              // ВХОД 5в СЕТЬ
#define IN_GENA_PIN 3              // ВХОД 5в ГЕНЕРАТОР
#define OUT_IGNITION_PIN 4         // РЕЛЕ ЗАЖИГАНИЯ
#define OUT_PODSOS_ON_PIN 5        // РЕЛЕ ВКЛЮЧЕНИЯ ПОДСОСА
#define OUT_STARTER_PIN 6          // РЕЛЕ СТАРТЕРА
#define OUT_PODSOS_OFF_PIN 7       // РЕЛЕ ВЫКЛЮЧЕНИЯ ПОДСОСА
#define OUT_KONTAKTOR_PIN 8        // РЕЛЕ КОНТАКТОРА (СИЛОВОЕ)
#define IN_STOP_PIN 10             // ВХОД 5в СТОП
#define LED_PIN 13                 // ДИОД НА ПЛАТЕ АРДУИНО "L"

// ВРЕМЕННЫЕ ЗАДЕРЖКИ В мс
#define TPODSOS_TIMEOUT 800        // ВРЕМЯ РАБОТЫ ПРИВОДА ПОДСОСА 0.8 с
#define STARTER_TIMEOUT 2000       // ВРЕМЯ РАБОТЫ СТАРТЕРА 2 с
#define OFF_PODSOS_TIMEOUT 2000    // ВРЕМЯ ДО ЗАКРЫТИЯ ПОДСОСА ПОСЛЕ ЗАПУСКА 2 с
#define PRE_KONT_TIMEOUT 15000     // ЗАДЕРЖКА ПЕРЕД ВКЛЮЧЕНИЕМ КОНТАКТОРА, ПОСЛЕ ЗАКРЫТИЯ ПОДСОСА (ПРОГРЕВ ДВИГАТЕЛЯ) 15 с
#define KONT_OFF_TIMEOUT 10000     // ЗАДЕРЖКА ПЕРЕД ВЫКЛЮЧЕНИЕМ КОНТАКТОРА, ПОСЛЕ ВОССТАНОВЛЕНИЯ СЕТИ 10 с
#define OFF_IGNITION_TIMEOUT 30000 // ЗАДЕРЖКА ВЫКЛЮЧЕНИЯ ЗАЖИГАНИЯ 30 с
#define OPERATION_TIMEOUT 500      // ЗАДЕРЖКА МЕЖДУ ОПЕРАЦИЯМИ ПРИ СТАРТЕ ГЕНЕРАТОРА 0.5 с
#define RELAX_TIMEOUT 5000         // ЗАДЕРЖКА МЕЖДУ ПУСКАМИ ДВИГАТЕЛЯ 5 с
#define LOOP_TIMEOUT 1000          // ЗАДЕРЖКА МЕЖДУ ОПРОСАМИ В ЦИКЛЕ 1 с

// ДРУГИЕ НАСТРОЙКИ
#define ZAPUSK_COUNTER_VAL 5       // КОЛИЧЕСТВО ПОПЫТОК ЗАПУСКА ГЕНЕРАТОРА


// ***КОНЕЦ НАСТРОЙКИ*** //


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


// настройка реле (не трогать)
// если реле управляется 1
#define HHH 1
#define LLL 0
// если реле управляется 0
//#define HHH 0
//#define LLL 1

// переменные значений входов
boolean chk_CETb;
boolean chk_GENA;
boolean chk_STOP;

// переменные
int starterCount = 0; // щетчик запусков

void blink(int bCount = 1){

	int blinkCount = bCount;

	for(int i=0; i<blinkCount; i++){
	    digitalWrite(LED_PIN, HIGH);
	    	delay(500);
	    digitalWrite(LED_PIN, LOW);
	    	delay(500);
	}

}

void errorMotorBlink(){  // ОШИБКА: двигатель не запустился после (n) попыток

	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
	digitalWrite(OUT_STARTER_PIN, LLL);
	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	digitalWrite(OUT_KONTAKTOR_PIN, LLL);
	if(digitalRead(OUT_IGNITION_PIN)){
		
	digitalWrite(OUT_IGNITION_PIN, LLL);
	}

	while(1){
	Serial.println("ERROR MOTOR");
		blink(5);
		delay(3000);
	}
}

void stopBlink(){  // АВАРИЙНЫЙ СТОП

	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
	digitalWrite(OUT_STARTER_PIN, LLL);
	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	digitalWrite(OUT_KONTAKTOR_PIN, LLL);
	if(digitalRead(OUT_IGNITION_PIN) == HHH){
		
		genStop();
	}


	while(1){
	Serial.println("ERROR STOP");
		blink(2);
		delay(1000);
	}
}


void genStart(){

	starterCount = 0;

	Serial.println("START GENERATORA");
	Serial.println("1");
	blink(3);
	digitalWrite(OUT_IGNITION_PIN, HHH);
		delay(OPERATION_TIMEOUT);
  Serial.println("podsos on");
	digitalWrite(OUT_PODSOS_ON_PIN, HHH);
		delay(TPODSOS_TIMEOUT);
	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
		delay(OPERATION_TIMEOUT);
	while(starterCount < ZAPUSK_COUNTER_VAL - 1){

	    blink(2);
	    Serial.println("STARTER ON"); 
	    digitalWrite(OUT_STARTER_PIN, HHH);
			delay(STARTER_TIMEOUT);
		digitalWrite(OUT_STARTER_PIN, LLL);
		Serial.println("STARTER OFF"); 

			delay(OFF_PODSOS_TIMEOUT);
     
		chk_GENA = digitalRead(IN_GENA_PIN);
    
		if(chk_GENA == HIGH){
  
	    	digitalWrite(OUT_PODSOS_OFF_PIN, HHH);
	        	delay(TPODSOS_TIMEOUT);
	    	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	    	starterCount = ZAPUSK_COUNTER_VAL + 1;
        Serial.print("START OK  ");
        Serial.println(starterCount + 1);
        Serial.println("podsos off");
		}else{

			blink();
			starterCount ++;
			delay(RELAX_TIMEOUT);
			Serial.println(starterCount+1);
		}
	}

	chk_GENA = digitalRead(IN_GENA_PIN);
	if(chk_GENA == LOW){
		delay(5);
		chk_GENA = digitalRead(IN_GENA_PIN);
	    if(chk_GENA == LOW){
	    	errorMotorBlink();
		}
	}


}

void genStop(){

	blink(4);
	Serial.print(": ");
    Serial.print(OFF_IGNITION_TIMEOUT / 1000);
    Serial.println(" c");
		delay(OFF_IGNITION_TIMEOUT);
	blink(2);
	Serial.println("STOP GENERATORA");
	digitalWrite(OUT_IGNITION_PIN, LLL);
}

void setup() {

	pinMode(IN_CETb_PIN, INPUT);
	pinMode(IN_GENA_PIN, INPUT);
	pinMode(OUT_IGNITION_PIN, OUTPUT);
	pinMode(OUT_PODSOS_ON_PIN, OUTPUT);
	pinMode(OUT_STARTER_PIN, OUTPUT);
	pinMode(OUT_PODSOS_OFF_PIN, OUTPUT);
	pinMode(OUT_KONTAKTOR_PIN, OUTPUT);
	pinMode(IN_STOP_PIN, INPUT_PULLUP);
	pinMode(LED_PIN, OUTPUT);

	digitalWrite(OUT_IGNITION_PIN, LLL);
	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
	digitalWrite(OUT_STARTER_PIN, LLL);
	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	digitalWrite(OUT_KONTAKTOR_PIN, LLL);
	digitalWrite(LED_PIN, LOW);

	Serial.begin(9600);
	Serial.println("AUTO GENERATOR");
	Serial.println("https://github.com/ss2s/AutoGeneratorArduino");
}

void loop() {

	blink();

	chk_CETb = digitalRead(IN_CETb_PIN);
	chk_GENA = digitalRead(IN_GENA_PIN);
	chk_STOP = digitalRead(IN_STOP_PIN);

  Serial.print("CETb ");
  Serial.print(chk_CETb);
  Serial.print("  GENA ");
  Serial.print(chk_GENA);
  Serial.print("  STOP ");
  Serial.println(chk_STOP);

	if(chk_CETb == LOW && chk_GENA == LOW && chk_STOP == HIGH){

			delay(5);
		chk_CETb = digitalRead(IN_CETb_PIN);
	    chk_GENA = digitalRead(IN_GENA_PIN);
	    chk_STOP = digitalRead(IN_STOP_PIN);

	    if(chk_CETb == LOW && chk_GENA == LOW && chk_STOP == HIGH){

	    	genStart();
	    	chk_CETb = digitalRead(IN_CETb_PIN);
			chk_GENA = digitalRead(IN_GENA_PIN);
			chk_STOP = digitalRead(IN_STOP_PIN);
			if(chk_CETb == LOW && chk_GENA == HIGH && chk_STOP == HIGH){ 
					delay(5);
				chk_CETb = digitalRead(IN_CETb_PIN);
				chk_GENA = digitalRead(IN_GENA_PIN);
				chk_STOP = digitalRead(IN_STOP_PIN);
				if(chk_CETb == LOW && chk_GENA == HIGH && chk_STOP == HIGH){
					Serial.print(": ");
    				Serial.print(PRE_KONT_TIMEOUT / 1000);
    				Serial.println(" c");
						delay(PRE_KONT_TIMEOUT);
					digitalWrite(OUT_KONTAKTOR_PIN, HHH);
						Serial.println("CONTAKTOR ON");
   
				} 
				else if(chk_CETb == HIGH && chk_GENA == HIGH){
					genStop();
				} 
			}
		}
	}else if(chk_CETb == HIGH && chk_GENA == HIGH && chk_STOP == HIGH){

			delay(5);
		chk_CETb = digitalRead(IN_CETb_PIN);
	    chk_GENA = digitalRead(IN_GENA_PIN);
	    chk_STOP = digitalRead(IN_STOP_PIN);

		if(chk_CETb == HIGH && chk_GENA == HIGH && chk_STOP == HIGH){

        Serial.println("poyavilas CETb");
        Serial.print(": ");
        Serial.print(KONT_OFF_TIMEOUT / 1000);
        Serial.println(" c");
		    delay(KONT_OFF_TIMEOUT);
		    digitalWrite(OUT_KONTAKTOR_PIN, LLL);
		    	Serial.println("KONATAKTOR OFF");

		    genStop();
		}
	}else if(chk_STOP == LOW){
		
		chk_STOP = digitalRead(IN_STOP_PIN);

		if(chk_STOP == LOW){

				delay(5);
	    	chk_STOP = digitalRead(IN_STOP_PIN);

	    	if(chk_STOP == LOW){
	    		stopBlink();
	    	}
		}
	}
	
	delay(LOOP_TIMEOUT);
}

