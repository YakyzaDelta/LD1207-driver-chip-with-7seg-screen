/****************************************************/
/* Simple Clock for ESP32 with millis() timer       */
/****************************************************/

#define VFD_in  23  // Data input
#define VFD_clk 18  // Clock
#define VFD_stb 19  // Strobe

// Global time variables
unsigned long previousMillis = millis();
unsigned char secs = 0;
unsigned char mins = 0;
unsigned char hrs  = 0;

// Character table for display (0-9 and colon at index 10)
unsigned char segNumber[11] = {
     (0b00111111), // 0
     (0b00110000), // 1
     (0b01011011), // 2
     (0b01111001), // 3
     (0b01110100), // 4
     (0b01101101), // 5
     (0b01101111), // 6
     (0b00111000), // 7
     (0b01111111), // 8
     (0b01111101), // 9
     (0b01000000), // Index 10: Colon
};

unsigned char gridSegments = 0b00000001; // Grid configuration

// Function prototypes
void send_command(unsigned char a);
void send_data8(unsigned char a);
void LD1207_init(void);
void write_panel_DVD(void);

void setup() {
  Serial.begin(115200);

  // Configure ESP32 pins as outputs
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_stb, OUTPUT);

  // Initialize the display
  LD1207_init();
  
  // Initial time (can be changed at startup)
  hrs = 12;
  mins = 0;
  secs = 0;
}

void loop() {
  // Timekeeping using millis() without AVR timer interrupts
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    secs++;
    
    if (secs >= 60) {
      secs = 0;
      mins++;
    }
    if (mins >= 60) {
      mins = 0;
      hrs++;
    }
    if (hrs >= 24) {
      hrs = 0;
    }
  }

  // Update the display
  write_panel_DVD();
}

// Display driver initialization
void LD1207_init(void) {
  delayMicroseconds(200);
  send_command(gridSegments);
  delayMicroseconds(1);
  send_command(0b01000000); // Write to memory with auto-increment mode
  delayMicroseconds(1);
  send_command(0b11000000); // Starting address
  delayMicroseconds(1);
  send_command((0b10001000) | 7); // Display ON, maximum brightness
  delayMicroseconds(1);
}

// Send 8 bits of data
void send_data8(unsigned char a) {
  unsigned char mask = 1;
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(2);
  
  for (mask = 0b00000001; mask > 0; mask <<= 1) {
    digitalWrite(VFD_clk, LOW);
    if (a & mask) {
      digitalWrite(VFD_in, HIGH);
    } else {
      digitalWrite(VFD_in, LOW);
    }
    delayMicroseconds(5);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(5);
  }
}

// Send a command packet
void send_command(unsigned char a) {
  unsigned char mask = 1;
  digitalWrite(VFD_stb, HIGH);
  delayMicroseconds(1);
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(1);
  
  for (mask = 0b00000001; mask > 0; mask <<= 1) {
    digitalWrite(VFD_clk, LOW);
    delayMicroseconds(1);
    if (a & mask) {
      digitalWrite(VFD_in, HIGH);
    } else {
      digitalWrite(VFD_in, LOW);
    }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
  }
  digitalWrite(VFD_stb, HIGH);
  delayMicroseconds(1);
}

// Function to display time on the VFD panel
void write_panel_DVD() {
  // Extract individual digits from minutes and hours
  unsigned char d0 = mins % 10; // Minutes units
  unsigned char c0 = mins / 10; // Minutes tens
  unsigned char b0 = hrs % 10;  // Hours units
  unsigned char a0 = hrs / 10;  // Hours tens

  send_command(gridSegments);
  delayMicroseconds(4);
  send_command(0b01000000); // Auto-increment mode
  delayMicroseconds(4);
  send_data8(0b11000000);   // Start from address 0
  delayMicroseconds(4); 

  // --- Grid 0 (animated snake pattern) ---
  // Snake animation pattern array - defines different frames for the animation
  const unsigned char snake[] = {0b00011000, 0b00001100, 0b01000100, 0b01100000, 0b00100001, 0b00000011, 0b01000010, 0b01010000};
  int snakeIndex = (millis() / (1000 / 8)) % 8; // Frame change 8 times per second
  send_data8(~snake[snakeIndex]); // Send inverted snake pattern data
  send_data8(0xFF); // Send all ones (brightness control for grid)

  // --- Grid 1 (colon display) ---
  // Check seconds: if even - colon ON, if odd - colon OFF
  if (secs % 2 == 0) {
    send_data8(~(segNumber[d0] | 0b10000000)); // Colon ON (bit 7 set)
    send_data8(~0b10000000);                   // Send inverse of colon bit
  } else {
    send_data8(~segNumber[d0]);                // Colon OFF
    send_data8(0b11111111);                    // Send all ones (no colon)
  }

  // --- Grid 2 (minutes tens digit) ---
  send_data8(~segNumber[c0]); 
  send_data8(0b00000000); // Grid control

  // --- Grid 3 (hours units digit) ---
  send_data8(~segNumber[b0]); 
  send_data8(0b00000000); // Grid control
  
  // --- Grid 4 (hours tens digit) ---
  send_data8(~segNumber[a0]); 
  send_data8(0b00000000); // Grid control

  send_command((0b10001000) | 7); // Re-enable display with max brightness
  delayMicroseconds(2);         
}