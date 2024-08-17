#define DDR_KEYPAD DDRK
#define PORT_KEYPAD PORTK
#define PIN_KEYPAD PINK






class SegmentDisplay {
public:
  byte* segments;
  byte* numbers;
  byte numberOfDigits;
  byte* value;
  SegmentDisplay(byte segments[8], byte numbers[], byte numberOfDigits)
    : segments(segments), numbers(numbers), numberOfDigits(numberOfDigits) {
    
      value = new byte[numberOfDigits];
  }

  virtual ~SegmentDisplay() {
      delete value;
  }

  void init() {
      for(int i = 0; i < 8;  i++) 
        pinMode(segments[i], OUTPUT);

      for(int i = 0; i < numberOfDigits;  i++) {
        pinMode(numbers[i], OUTPUT);
        digitalWrite(numbers[i], HIGH);
      }
  }

  void setValue(byte value[]) {
    for(int i = 0; i < numberOfDigits;  i++) {
      this->value[i] = value[i];
    }
  }


  void update() {
    byte actualCounter = min(counter, 7);
    for(int i = 0; i < numberOfDigits; i++)
        digitalWrite(numbers[i], i == actualCounter ? LOW : HIGH);
    

    for(int i = 0;  i < 8; i++)
      digitalWrite(segments[i], (value[actualCounter] >> i) & 1 ? HIGH : LOW);

      
    counter = (counter + 1) % (numberOfDigits + 2);
  }

  private:
    byte counter = 0;
};

void getSegmentFromTime(byte * segment, long time, bool alarmActive) {
    long hours = (time / (60 * 60)) % 24;
    long minutes = (time / 60) % 60;
    long seconds = time % 60;
    long timeBySymbol[] = { hours / 10,  hours % 10, minutes / 10,  minutes % 10, seconds / 10, seconds % 10 };

    for(int i = 0; i < 6; i++) {
      //                                      .gfedcba
      if(timeBySymbol[i] == 0) segment[i] = 0b00111111;
      if(timeBySymbol[i] == 1) segment[i] = 0b00000110;
      if(timeBySymbol[i] == 2) segment[i] = 0b01011011;
      if(timeBySymbol[i] == 3) segment[i] = 0b01001111;
      if(timeBySymbol[i] == 4) segment[i] = 0b01100110;
      if(timeBySymbol[i] == 5) segment[i] = 0b01101101;
      if(timeBySymbol[i] == 6) segment[i] = 0b01111101;
      if(timeBySymbol[i] == 7) segment[i] = 0b00000111;
      if(timeBySymbol[i] == 8) segment[i] = 0b01111111;
      if(timeBySymbol[i] == 9) segment[i] = 0b01101111;
    }

    if(alarmActive)            segment[5] = 0b01110111;

    segment[1] |= 0b10000000;
    segment[3] |= 0b10000000;
}

