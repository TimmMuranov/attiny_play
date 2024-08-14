//===========Макросы============
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define F_CPU 1000000UL  // 1 MHz

#define p0_high PORTB|=(1<<PB0)
#define p1_high PORTB|=(1<<PB1)
#define p2_high PORTB|=(1<<PB2)
#define p0_low PORTB&=~(1<<PB0)
#define p1_low PORTB&=~(1<<PB1)
#define p2_low PORTB&=~(1<<PB2)

#define p3_read PINB&(1<<PINB3)
#define p4_read PINB&(1<<PINB4)

//проценты покрытия магнитного поля
#define percent_const 5 //const датчик

//== Настройка секундного таймера ===
volatile int timer_sec = 0;

ISR(TIMER0_COMPA_vect) {
   timer_sec++;
   if (x==255){x=0;}       
}

//====== Функция декодирования ====
//7-й вывод (1-1-1 на входе) висит
void decoder (int in) {
   switch (in){
      case 0:
         p0_high; p1_high; p2_high; break;
      case 1:
         p0_low; p1_low; p2_low; break;
      case 2:
         p0_high; p1_low; p2_low; break;
     case 3:
         p0_low; p1_high; p2_low; break;
     case 4:
         p0_high; p1_high; p2_low; break;
     case 5:
         p0_low; p1_low; p2_high; break;
     case 6:
         p0_high; p1_low; p2_high; break;
     case 7:
         p0_low; p1_high; p2_high; break;
     }
}
   
//===== Случайное число =========
int lfsr(int in) {
    return lfsr_8bit((uint8_t)in);
}

uint8_t lfsr_8bit(uint8_t in) {
    uint8_t bit = 
    ((in >> 0) ^ (in >> 2) ^ 
    (in >> 3) ^ (in >> 4)) & 1;
    in = (in >> 1) | (bit << 7);
    return in;
}
//====== Настройка регистров =====

TCCR0B |= (1 << WGM02) | (1 << CS02) | (1 << CS00);
TIMSK |= (1 << OCIE0A);
sei();

DDRB |= (1 << PB0);
DDRB |= (1 << PB1);
DDRB |= (1 << PB2);
DDRB &= ~(1 << PB3);
DDRB &= ~(1 << PB4);

//===== Переменные индикации =====

unsigned long t = 0;
unsigned long time = 0; 
// переменные поворота

int b = 0;
int button = 0; // кнопка

unsigned int matrix[7][30]; 
//состояния светодиодов

int flag;
//флаг успешной установки времени

//======== Переменные игры =======
int snake_x=0;
int snake_y=0;
//Координаты головы
//Х - по часовой (+), против часовой (-)
//Y - от центра (+), к центру (-)

int snake_size = 1;
//Размеры змейки

int snake_time = 0;

int snake_loc = 0;
//Направление движения змеи
// 0 - по часовой, 1 - внутрь
// 2 - против часовой, 3 - наружу

int flag_apple;
//Нужен для генерации яблока

while(1){
//===== Счет времени оборота ======

   while (p3_read==1){
      _delay_ms(1);
      t++; 
      if(p4_read==1){b = 1;}
      flag = 1;
      }

   time = (t/percent_const)*100;
   //время 1 оборота (мс)
   button += b; b = 0; 
// Индикация состояния кнопки

//====== Отображение экрана ======

for (int x=0; x<30; ++x){
   for (int y=1; y<=7; ++y){
      if(p3_read == 1){break;}
      if(matrix[y][x] > 0) {decoder(y);}
   }
   if(p3_read == 1){break;}
   _delay_ms(time/33);
  }


//==== Изменение массива (игра) ====
//Срабатывает каждую секунду
if(snake_time < time_sec){
   snake_time = time_sec;

//______________________________
//Обработка нажатий и зажатий кнопки

   if (button == 1 && (p4_read) == 0){
      snake_loc++;
      button = 0;
      if(snake_loc > 3){snake_loc = 0;}
   }

   if(button == 1 && (p4_read) == 1){
      snake_loc–;
      button = -1;
      if(snake_loc < 0){snake_loc = 3;}
   }
//______________________________
//Обработка поворотов змеи
//Зависит от направления snake_loc

switch (snake_loc){
      case 0:
         snake_x++;
         if (snake_x > 30){snake_x = 0;}
      case 1:
         snake_y–;
        if (snake_y < 0) {snake_y = 7;}
      case 2:
         snake_x–;
         if (snake_x < 0) {snake_x = 30;}
      case 3:
         snake_y++;
         if (snake_y > 7) {snake_y = 0;}
}

//_______________________________

//Декрементирование всего массива
//В случае отсутствия яблока у головы

//Если яблоко появилось в голове змеи
//Размер snake_loc инкрементируется

for(int y=0; y<7; ++y){
   for(int x=0; x<30; ++x){
      if(matrix[y][x] > 0){
         if(matrix[snake_y][snake_x]!=255]{
               –matrix[y][x];
         }
      }
   }
}
if(matrix[snake_y][snake_x]==255{
   snake_size++;
   flag_apple = 0;
}

//_____________________________
//Добавление головы змеи

matrix [snake_y][snake_x] = snake_size;

//======= Генерация яблока ========
//Если flag_apple на 0, идет генерация
while (flag_apple == 0) {
   int rand_y = lfsr(snake_time)/32+1;
   int rand_x = lfsr(rand_y)/8.5;
   if(matrix[rand_y][rand_x] == 0){
      matrix[rand_y][rand_x] = 255;
      flag_apple = 1;
   }
}
//==============================
    }
  }
}
