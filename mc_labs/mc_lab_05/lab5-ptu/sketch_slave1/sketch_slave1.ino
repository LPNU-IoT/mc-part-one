#define B 1
#define U 2
#define C 3
#define H 4
#define A 5
#define K 6
#define R 7
#define O 8
#define M 9
#define N 10
#define T 11
#define S 12
#define V 13
#define Y 14
#define _ 15

const byte SLAVE_ADDRESS = 55;
const byte DATA_ARRAY_LENGTH = 24;

byte data_array[DATA_ARRAY_LENGTH] = {
  B, U, C, H, A, K, _,
  R, O, M, A, N, _, 
  T, A, R, A, S, O, V, Y, C, H,
  _
};

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

ISR(USART_TX_vect) { 
  PORTD &= ~(1 << PD2);
}

int writeDataToMaster() {
  unsigned short checkSumCRC = get_crc8(data_array, DATA_ARRAY_LENGTH - 1);
  data_array[DATA_ARRAY_LENGTH - 1] = checkSumCRC;

  for(int k = 0; k < 5; k++){
    for (int i = 0; i < DATA_ARRAY_LENGTH; i++) {
        byte byteToSend = data_array[i];
        if (k == 1 && i == DATA_ARRAY_LENGTH - 2) {
          byteToSend ^= (1 << 0);
        } else if (k == 4 && i == 3) {
          byteToSend ^= (1 << 0) | (1 << 2) | (1 << 5);
        }
        Serial.write(byteToSend);
    }
  }
}

void setup() {
  delay(200);
  DDRD = 0b00000111;
  PORTD = 0b11111000;
  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);
  delay(1);
}

void loop() {
  if (Serial.available()) {
    byte inByte = Serial.read();
    if (SLAVE_ADDRESS == inByte) {
      UCSR0A &= ~(1 << MPCM0);
      setWriteModeRS485();
      writeDataToMaster();
      delay(100);
    } 
  }
}

uint8_t get_crc8(const uint8_t *data, size_t array_length) {
    const uint8_t poly = 0x7;
    uint8_t crc = 0x00;

    for (size_t i = 0; i < array_length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc ^ 0x00;
}
