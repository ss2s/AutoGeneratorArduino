# AutoGeneratorArduino

#### created by ss2s
<https://github.com/ss2s>

```
// АВТОЗАПУСК ГЕНЕРАТОРА на Arduino
// АВР автоматический ввод резерва
// created by ss2s
// https://github.com/ss2s
// https://github.com/ss2s/AutoGeneratorArduino

// КАК ПОДКЛЮЧАТЬ:
// Подключите реле как указано в разделе распиновка (можно изменить в разделе настройка:распиновка)
// Вход 5в от сети и генератора подключять через РЕЛЕ . тоесть реле перекидывает контакт между 5в и 0в земля.
// по умолчанию: 5в сеть есть. сеть пропала реле перекинуло на 0в.

// Кнопка СТОП програмно подтянута к 5в. для активации замкнуть на землю(можно через реле нормально разомкнутое)
// кнопка СТОП предназначена для предотвращения запуска генератора в случае аварии в сети. например если МЧС отключит свет

// провода использовать экранированые и как можно короче (для защиты от наводок)
// микроконтроллер желательно разместить в заземленном экранированом корпусе.

// в разделе настройка:временные задержки - можно настроить временные задержки в миллисекундах. 1000 мс = 1 с

// СВЕТОДИОДНАЯ ИНДИКАЦИЯ
// мигает раз в секунду - нормальный режим работы. мониторит сеть.
// 3 раза мигает пред запуском генератора и по 2 раза перед каждой последующей попыткой. если с первого раза не запустился.
// 4 раза мигает за 30сек. до выключения зажигания и 2 раза перед выключением.
// 5 миганий 3сек. пауза - ошибка. двигатель не запустился после n попыток.
// 2 мигание 1сек. пауза - ошибка. пришел сигнал СТОП.


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


// ***НАСТРОЙКА*** //

// для настройки изменять цифры
// ВХОДЫ РЕЛЕ 5в СЕТЬ И 5в ГЕНЕРАТОР ПОДКЛЮЧАТЬ ЧЕРЕЗ ПЕРЕКИДНОЕ РЕЛЕ(5в СЕТЬ ЕСТЬ. 0в СЕТИ НЕТ)
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
```
